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
	m_hThread = NULL;
	m_hExitEvent = NULL;
	m_hDirectory = INVALID_HANDLE_VALUE;
	memset(m_filePath,0,MAX_PATH);
}

CCheckDirChange::~CCheckDirChange()
{
	StopCheck();
}

void CCheckDirChange::Init(const char* lpDirPath)
{
	m_changeFile.Init();

	if (NULL == m_hThread&&OpenDirectory(lpDirPath))
	{
		strcpy(m_filePath,lpDirPath);
		m_hExitEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
		m_hThread = (HANDLE)_beginthreadex(NULL,0,NotifiDirChangeThread,this,0,NULL);
	}
}

void CCheckDirChange::StopCheck()
{
	if (m_hThread)
	{
		SetEvent(m_hExitEvent);
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread,2000))
		{
			TerminateThread(m_hThread,0);
		}
		
		CloseHandle(m_hThread);
		m_hThread = NULL;
		
		CloseHandle(m_hExitEvent);
		
		if (m_hDirectory!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hDirectory);
			m_hDirectory = INVALID_HANDLE_VALUE;
		}
	}

	m_changeFile.Stop();
}

UINT __stdcall CCheckDirChange::NotifiDirChangeThread(LPVOID	lpParam)
{
	CCheckDirChange* pThis = (CCheckDirChange*) lpParam;
	OVERLAPPED	overlapped = {0};
	HANDLE	hWaitEvent[2]={0};
	DWORD	dwMaxBufferLen = 4096,dwReturnLen,dwWait;
	char	csNotifyBuffer[4096] = {0};    
	FILE_NOTIFY_INFORMATION *pFileNotification=(FILE_NOTIFY_INFORMATION *)csNotifyBuffer;

	overlapped.hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
	hWaitEvent[0] = pThis->m_hExitEvent;
	hWaitEvent[1] = overlapped.hEvent;

	while(1)
	{
		memset(csNotifyBuffer,0,dwMaxBufferLen);
		if (!ReadDirectoryChangesW(pThis->m_hDirectory,
									pFileNotification,
									dwMaxBufferLen,
									FALSE,
									FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE, 
									&dwReturnLen,
									&overlapped,
									NULL))
		{
				break;
		}

		dwWait = WaitForMultipleObjects(2,hWaitEvent,FALSE,INFINITE);
		if ((WAIT_OBJECT_0+1) == dwWait)
		{
			pThis->AnalyDirChangeEvent(pFileNotification);
		}
		else if (WAIT_OBJECT_0 == dwWait)
		{
			break;
		}
	}


	return 0;
}

bool CCheckDirChange::OpenDirectory(const char* lpDirPath)
{
	m_hDirectory = CreateFile(lpDirPath,
								FILE_LIST_DIRECTORY,
								FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
								NULL,
								OPEN_EXISTING,
								FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
								NULL); 

	return (m_hDirectory!=INVALID_HANDLE_VALUE)?true:false;
}

void CCheckDirChange::AnalyDirChangeEvent(PFILE_NOTIFY_INFORMATION pFileNotify)
{
    DWORD cbOffset;

	do 
	{
		cbOffset = pFileNotify->NextEntryOffset;
		
		if(FILE_ACTION_ADDED == pFileNotify->Action)
			CheckAddedFile(  pFileNotify );
		
		pFileNotify = (PFILE_NOTIFY_INFORMATION)((LPBYTE) pFileNotify + cbOffset);

     } while( cbOffset );
}

void CCheckDirChange::CheckAddedFile(PFILE_NOTIFY_INFORMATION pFileNotify)
{
    TCHAR      szFileName[MAX_PATH]={0};
	char	   csFileFullNames[MAX_PATH]={0};
	
    memcpy( szFileName, pFileNotify->FileName ,pFileNotify->FileNameLength);
    szFileName[pFileNotify->FileNameLength/sizeof(TCHAR)]=0;

	::WideCharToMultiByte(CP_ACP,0,(unsigned short*)szFileName, pFileNotify->FileNameLength, csFileFullNames, MAX_PATH,NULL,NULL); 
	m_changeFile.AddItem(csFileFullNames);
}