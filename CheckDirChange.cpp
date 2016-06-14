// CheckDirChange.cpp: implementation of the CCheckDirChange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestDirChange.h"
#include "CheckDirChange.h"
#include <process.h>
#include <windows.h>

#define POST_READ_LENTH 5

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

void CCheckDirChange::ChangeEventAndChangeInfo(PReadDirChangeInfo pChangeInfos[],HANDLE hWatchEvent[],int length,int FireEventIndexs[],int FireEventNum)
{
	HANDLE FireEvents[POST_READ_LENTH] = {0};
	PReadDirChangeInfo pFireChangeInfos[POST_READ_LENTH] = {0};

	int d=FireEventIndexs[0],s=d+1;
	if(s<length)
	{
		for (int i=0;i<FireEventNum;++i)
		{
			int endindex = length;
			FireEvents[i] = hWatchEvent[FireEventIndexs[i]];
			pFireChangeInfos[i] = pChangeInfos[FireEventIndexs[i]];
			if(i+1<FireEventNum)
				endindex = FireEventIndexs[i+1];
			for (int j=FireEventIndexs[i]+1;j<endindex;++j)
			{
				pChangeInfos[d] = pChangeInfos[s];
				hWatchEvent[d++]=hWatchEvent[s++];				
			}			
			++s;
		}
		for (int i=0;i<FireEventNum;++i)
		{
			pChangeInfos[d] = pFireChangeInfos[i];
			hWatchEvent[d++] = FireEvents[i];
		}
	}
}

UINT __stdcall CCheckDirChange::NotifiDirChangeThread(LPVOID	lpParam)
{
	CCheckDirChange*			pThis = (CCheckDirChange*) lpParam;
	HANDLE						hWaitEvent[2]={0},hWatchEvent[POST_READ_LENTH+1]={0};
	const DWORD					dwMaxBufferLen = 8192;
	DWORD						dwWait,dwIndex;
	PReadDirChangeInfo			pReadDirChangeInfo[POST_READ_LENTH]={0};
	hWaitEvent[0] = pThis->m_hBeginWatchEvent;
	hWaitEvent[1] = pThis->m_hSempEvent[1];

	for (int i=0;i<POST_READ_LENTH;++i)
	{
		pReadDirChangeInfo[i] = new ReadDirChangeInfo;
		pReadDirChangeInfo[i]->overlapped.hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
		hWatchEvent[i] = pReadDirChangeInfo[i]->overlapped.hEvent;
		pReadDirChangeInfo[i]->dwMaxBufferLen = dwMaxBufferLen;
		pReadDirChangeInfo[i]->pFileNotification = NULL;
	}

	hWatchEvent[POST_READ_LENTH] = pThis->m_hExitWatchEvent;

	int FireEventNum = 0;
	int FireEventIndexs[POST_READ_LENTH] = {0};
	int lastIndex = 0;
	HANDLE FireEvnets[POST_READ_LENTH] = {0};
	bool BreakWatch = false;
	while (1)
	{
		dwWait = WaitForMultipleObjects(2,hWaitEvent,FALSE,INFINITE);
		if (WAIT_OBJECT_0 == dwWait)
		{
			if (pThis->OpenDirectory())
			{
				for (int i=0;i<POST_READ_LENTH;++i)
					ResetEvent(pReadDirChangeInfo[i]->overlapped.hEvent);

				if(pThis->WatchDirChanges(pReadDirChangeInfo,POST_READ_LENTH))
				{
					while(1)
					{
						dwWait = WaitForMultipleObjects(POST_READ_LENTH+1,hWatchEvent,FALSE,INFINITE);
						dwIndex = dwWait - WAIT_OBJECT_0;
						FireEventNum = 0;
						BreakWatch = false;
						if (dwIndex<POST_READ_LENTH)
						{
							pThis->PushFileNotifyInfo(pReadDirChangeInfo[dwIndex]->pFileNotification);							
							pThis->WatchDirChanges(pReadDirChangeInfo[dwIndex]);
							FireEventIndexs[FireEventNum++] = dwIndex;
							
							while(dwIndex<POST_READ_LENTH)
							{
								lastIndex = dwIndex;
								dwWait = WaitForMultipleObjects(POST_READ_LENTH-dwIndex,&(hWatchEvent[dwIndex+1]),FALSE,0);
								dwIndex = dwWait - WAIT_OBJECT_0;
								dwIndex +=lastIndex+1;
								if(dwIndex<POST_READ_LENTH)
								{
									pThis->PushFileNotifyInfo(pReadDirChangeInfo[dwIndex]->pFileNotification);							
									pThis->WatchDirChanges(pReadDirChangeInfo[dwIndex]);
									FireEventIndexs[FireEventNum++] = dwIndex;
								}
								else if(dwIndex == POST_READ_LENTH)
								{
									BreakWatch = true;
									break;
								}

							}		
							if(BreakWatch)
								break;			
							pThis->ChangeEventAndChangeInfo(pReadDirChangeInfo,hWatchEvent,POST_READ_LENTH,FireEventIndexs,FireEventNum);
						}
						else
						{
							break;
						}
					}
				}

				for (int i=0;i<POST_READ_LENTH;++i)
					if (pReadDirChangeInfo[i]->pFileNotification)
						delete [] (char*)pReadDirChangeInfo[i]->pFileNotification;

			}

			pThis->CloseDirectory();
		}
		else if (WAIT_OBJECT_0+1 == dwWait)
		{
			break;
		}
	}
	for (int i=0;i<POST_READ_LENTH;++i)
	{
		CloseHandle(pReadDirChangeInfo[i]->overlapped.hEvent);
		delete pReadDirChangeInfo[i];
		pReadDirChangeInfo[i] = NULL;
	}

	return 0;
}



bool CCheckDirChange::WatchDirChanges(PReadDirChangeInfo pChangeInfo)
{
	pChangeInfo->pFileNotification = (FILE_NOTIFY_INFORMATION*)(new char[pChangeInfo->dwMaxBufferLen]);
	memset(pChangeInfo->pFileNotification,0,pChangeInfo->dwMaxBufferLen);

	return ReadDirectoryChangesW(m_hDirectory,
		pChangeInfo->pFileNotification,
		pChangeInfo->dwMaxBufferLen,
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_DIR_NAME, 
		&pChangeInfo->dwReturnLen,
		&pChangeInfo->overlapped,
		NULL)?true:false;
}

bool CCheckDirChange::WatchDirChanges(PReadDirChangeInfo pChangeInfos[] ,int length)
{
	for (int i=0;i<length;++i)
	{
		pChangeInfos[i]->pFileNotification = (FILE_NOTIFY_INFORMATION*)(new char[pChangeInfos[i]->dwMaxBufferLen]);
		memset(pChangeInfos[i]->pFileNotification,0,pChangeInfos[i]->dwMaxBufferLen);
		bool ret = ReadDirectoryChangesW(m_hDirectory,
			pChangeInfos[i]->pFileNotification,
			pChangeInfos[i]->dwMaxBufferLen,
			TRUE,
			FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_DIR_NAME, 
			&pChangeInfos[i]->dwReturnLen,
			&pChangeInfos[i]->overlapped,
			NULL)?true:false;
		if(ret == false)
			return false;
	}
	return true;
}

void CCheckDirChange::PushFileNotifyInfo(PFILE_NOTIFY_INFORMATION pFileNotify)
{
	m_listLock.Lock();
	m_notifyInfoList.push_back(pFileNotify);
	m_listLock.Unlock();

	ReleaseSemaphore(m_hSempEvent[0],1,NULL);
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