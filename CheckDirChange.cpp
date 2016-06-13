// CheckDirChange.cpp: implementation of the CCheckDirChange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestDirChange.h"
#include "CheckDirChange.h"
#include <process.h>
#include <windows.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#pragma comment(lib,"Kernel32.lib")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCheckDirChange::CCheckDirChange()
{
	m_hThread[0] = NULL;
	m_hThread[1] = NULL;
	m_hExitWatchEvent = NULL;
	m_hDirectory = INVALID_HANDLE_VALUE;

	m_hBeginWatchEvent = NULL;

	m_hSempEvent[0] = NULL;
	m_hSempEvent[1] = NULL;

	memset(m_csWatchFilePath,0,MAX_PATH);
}

CCheckDirChange::~CCheckDirChange()
{
	StopCheck();
	UnInit();
}

void CCheckDirChange::Init()
{
	m_changeFile.Init();

	if (NULL==m_hThread[0]&&NULL==m_hThread[1])
	{
		m_hBeginWatchEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
		m_hExitWatchEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
		
		m_hSempEvent[0] = ::CreateSemaphore(NULL,0,0x7FFFFF,NULL);
		m_hSempEvent[1] = ::CreateEvent(NULL,TRUE,FALSE,NULL);	
		
		m_hThread[1] = (HANDLE)_beginthreadex(NULL,0,AnalyDirChangeThread,this,0,NULL);
		m_hThread[0] = (HANDLE)_beginthreadex(NULL,0,NotifiDirChangeThread,this,0,NULL);
	}
}

void CCheckDirChange::UnInit()
{
	if (m_hThread[1]||m_hThread[0])
	{
		SetEvent(m_hSempEvent[1]);
		WaitForMultipleObjects(2,m_hThread,TRUE,1000);
		
		CloseHandle(m_hThread[1]);
		m_hThread[1] = NULL;

		CloseHandle(m_hThread[0]);
		m_hThread[0] = NULL;

		CloseHandle(m_hExitWatchEvent);
		CloseHandle(m_hBeginWatchEvent);
		CloseHandle(m_hSempEvent[0]);
		CloseHandle(m_hSempEvent[1]);
	}

	m_changeFile.Stop();
}

bool CCheckDirChange::StartCheck(const char* lpDirPath)
{
	if (m_hBeginWatchEvent)
	{
		m_changeFile.Start();
		strcpy(m_csWatchFilePath,lpDirPath);
		SetEvent(m_hBeginWatchEvent);
		
		return true;
	}

	return false;
}

void CCheckDirChange::StopCheck()
{
	if (m_hExitWatchEvent)
	{
		SetEvent(m_hExitWatchEvent);
	}
}

UINT __stdcall CCheckDirChange::NotifiDirChangeThread(LPVOID	lpParam)
{
	CCheckDirChange*			pThis = (CCheckDirChange*) lpParam;
	OVERLAPPED					overlapped = {0};
	HANDLE						hWaitEvent[2]={0},hWatchEvent[2]={0};
	const DWORD					dwMaxBufferLen = 8192;
	DWORD						dwReturnLen,dwWait;
    char*						pNotifyBuffer;
	FILE_NOTIFY_INFORMATION *	pFileNotification;

	hWaitEvent[0] = pThis->m_hBeginWatchEvent;
	hWaitEvent[1] = pThis->m_hSempEvent[1];

	overlapped.hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
	hWatchEvent[0] = overlapped.hEvent;
	hWatchEvent[1] = pThis->m_hExitWatchEvent;

	pNotifyBuffer = new char[dwMaxBufferLen];
	pFileNotification = (FILE_NOTIFY_INFORMATION *)pNotifyBuffer;

	while (1)
	{
		dwWait = WaitForMultipleObjects(2,hWaitEvent,FALSE,INFINITE);
		if (WAIT_OBJECT_0 == dwWait)
		{
			if (pThis->OpenDirectory())
			{
				memset(pNotifyBuffer,0,dwMaxBufferLen);
				ResetEvent(overlapped.hEvent);
				while(1)
				{	
					if (!ReadDirectoryChangesW(pThis->m_hDirectory,
						pFileNotification,
						dwMaxBufferLen,
						TRUE,
						FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_DIR_NAME, 
						&dwReturnLen,
						&overlapped,
						NULL))
					{
						break;
					}
					
					dwWait = WaitForMultipleObjects(2,hWatchEvent,FALSE,INFINITE);
					if (WAIT_OBJECT_0 == dwWait)
					{
						pThis->m_listLock.Lock();
						pThis->m_notifyInfoList.push_back(pFileNotification);
						pThis->m_listLock.Unlock();
						ReleaseSemaphore(pThis->m_hSempEvent[0],1,NULL);
						
						pNotifyBuffer = new char[dwMaxBufferLen];
						memset(pNotifyBuffer,0,dwMaxBufferLen);
						pFileNotification = (FILE_NOTIFY_INFORMATION *)pNotifyBuffer;
					}
					else if ((WAIT_OBJECT_0+1) == dwWait)
					{
						break;
					}
				}
			}

			pThis->CloseDirectory();
		}
		else if (WAIT_OBJECT_0+1 == dwWait)
		{
			break;
		}
	}

	CloseHandle(overlapped.hEvent);	
	delete [] pNotifyBuffer;

	return 0;
}

UINT __stdcall CCheckDirChange::AnalyDirChangeThread(LPVOID	lpParam)
{
	CCheckDirChange* pThis = (CCheckDirChange*) lpParam;
	DWORD	dwWait;
	while(1)
	{
		dwWait = WaitForMultipleObjects(2,pThis->m_hSempEvent,FALSE,INFINITE);
		if (WAIT_OBJECT_0 == dwWait)
		{
			PFILE_NOTIFY_INFORMATION& pNotify = pThis->m_notifyInfoList.front();
			pThis->AnalyDirChangeEvent(pNotify);
			delete [] ((char*)pNotify);

			pThis->m_listLock.Lock();
			pThis->m_notifyInfoList.pop_front();
			pThis->m_listLock.Unlock();
		}
		else if ((WAIT_OBJECT_0+1) == dwWait)
		{
			break;
		}
	}
	
	return 0;
}

bool CCheckDirChange::OpenDirectory()
{
	m_hDirectory = CreateFile(m_csWatchFilePath,
								FILE_LIST_DIRECTORY,
								FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
								NULL,
								OPEN_EXISTING,
								FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
								NULL); 

	return (m_hDirectory!=INVALID_HANDLE_VALUE)?true:false;
}

void CCheckDirChange::CloseDirectory()
{
	if (m_hDirectory!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDirectory);
		m_hDirectory = INVALID_HANDLE_VALUE;
	} 
}

void CCheckDirChange::AnalyDirChangeEvent(PFILE_NOTIFY_INFORMATION pFileNotify)
{
    DWORD cbOffset;
	
	do 
	{
		cbOffset = pFileNotify->NextEntryOffset;

		CheckAddedFile(  pFileNotify );

		pFileNotify = (PFILE_NOTIFY_INFORMATION)((LPBYTE) pFileNotify + cbOffset);

     } while( cbOffset );
}

void CCheckDirChange::CheckAddedFile(PFILE_NOTIFY_INFORMATION pFileNotify)
{
	memset(m_szFileName,0,MAX_PATH*sizeof(TCHAR));
	memcpy( m_szFileName, pFileNotify->FileName ,pFileNotify->FileNameLength);

	::WideCharToMultiByte(CP_ACP,0,(LPCWSTR)m_szFileName, pFileNotify->FileNameLength, m_csFileFullNames, MAX_PATH,NULL,NULL); 
	m_changeFile.AddItem(m_csFileFullNames,pFileNotify->Action);
}