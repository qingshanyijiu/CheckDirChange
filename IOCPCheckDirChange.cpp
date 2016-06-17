// CheckDirChange.cpp: implementation of the CIOCPCheckDirChange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestIOCPDirChange.h"
#include "IOCPCheckDirChange.h"
#include <process.h>
#include <windows.h>

#define  EXIT_CODE (0)


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#pragma comment(lib,"Kernel32.lib")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOCPCheckDirChange::CIOCPCheckDirChange()
{
	m_hDirectory =  INVALID_HANDLE_VALUE;
	m_hIOCompletionPort = INVALID_HANDLE_VALUE;
	m_phWorkThreads = NULL;
	m_hDisposeThread = NULL;
	m_pContexts = NULL;
	m_nMaxContextsCount = 0;
	m_nWorkerCount = 0;

	m_hSempEvent[0] = NULL;
	m_hSempEvent[1] = NULL;

	memset(m_csWatchFilePath,0,MAX_PATH);
}

CIOCPCheckDirChange::~CIOCPCheckDirChange()
{
	StopCheck();
	UnInit();
}

void CIOCPCheckDirChange::Init()
{
	m_changeFile.Init();

	InitDispose();
	InitIOCP();
}

void CIOCPCheckDirChange::UnInit()
{
	if (INVALID_HANDLE_VALUE != m_hIOCompletionPort)
	{
		int i;
		for (i=0;i<m_nWorkerCount;++i)
			PostQueuedCompletionStatus(m_hIOCompletionPort,0,DWORD(EXIT_CODE),NULL);
		WaitForMultipleObjects(m_nWorkerCount,m_phWorkThreads,TRUE,m_nWorkerCount*1000);

		for (i=0;i<m_nWorkerCount;++i)
			CloseHandle(m_phWorkThreads[i]);

		delete [] m_phWorkThreads;
		delete [] m_pContexts;
	}

	if (m_hDisposeThread)
	{
		SetEvent(m_hSempEvent[1]);
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hDisposeThread,1000))
			TerminateThread(m_hDisposeThread,0);
		
		CloseHandle(m_hDisposeThread);
		m_hDisposeThread = NULL;

		CloseHandle(m_hSempEvent[0]);
		CloseHandle(m_hSempEvent[1]);
	}

	m_changeFile.Stop();
}


void CIOCPCheckDirChange::InitIOCP()
{
	if ( INVALID_HANDLE_VALUE == m_hIOCompletionPort)
	{
		m_hIOCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );
		m_nWorkerCount = GetNoOfProcessors()+1;
		m_nMaxContextsCount  = 2*m_nWorkerCount+1;

		m_pContexts = new PER_IO_CONTEXT[m_nMaxContextsCount];
		m_phWorkThreads = new HANDLE[m_nWorkerCount];

		for (int i=0;i<m_nWorkerCount;++i)
			m_phWorkThreads[i] = (HANDLE)_beginthreadex(NULL,0,NotifiDirChangeThread,this,0,NULL); 
	}
}

void CIOCPCheckDirChange::InitDispose()
{
	if (NULL == m_hDisposeThread)
	{
		m_hSempEvent[0] = ::CreateSemaphore(NULL,0,0x7FFFFF,NULL);
		m_hSempEvent[1] = ::CreateEvent(NULL,TRUE,FALSE,NULL);	
		
		m_hDisposeThread = (HANDLE)_beginthreadex(NULL,0,AnalyDirChangeThread,this,0,NULL);
	}
}

int	 CIOCPCheckDirChange::GetNoOfProcessors()
{
	SYSTEM_INFO si;
	
	GetSystemInfo(&si);
	
	return si.dwNumberOfProcessors;
}

bool CIOCPCheckDirChange::PostWatchFileChange(PPER_IO_CONTEXT pContext)
{
	pContext->GetNewFileNotifyBuffer();
	
	return ReadDirectoryChangesW(m_hDirectory,
		pContext->pFileNotification,
		MAX_FILE_NOTIFY_LEN,
		TRUE,
		FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_DIR_NAME, 
		&pContext->dwReturnLen,
		&pContext->overlapped,
		NULL)?true:false;
}

bool CIOCPCheckDirChange::StartCheck(const char* lpDirPath)
{
	if (INVALID_HANDLE_VALUE == m_hDirectory)
	{
		m_changeFile.Start();
		strcpy(m_csWatchFilePath,lpDirPath);

		OpenDirectory();
		if (INVALID_HANDLE_VALUE != m_hDirectory)
		{
			m_hIOCompletionPort = CreateIoCompletionPort(m_hDirectory, m_hIOCompletionPort, (ULONG)m_hDirectory, 0 );

			for (int i=0;i<m_nMaxContextsCount;++i)
			{
				PostWatchFileChange(&m_pContexts[i]);
			}

		}
	}

	return (INVALID_HANDLE_VALUE!=m_hDirectory)?true:false;
}

void CIOCPCheckDirChange::StopCheck()
{
	CloseDirectory();
}

UINT __stdcall CIOCPCheckDirChange::NotifiDirChangeThread(LPVOID	lpParam)
{
	CIOCPCheckDirChange*	pThis = (CIOCPCheckDirChange*) lpParam;
	OVERLAPPED*				pOverlapped = NULL;
	DWORD					dwBytesTransfered = 0;
	HANDLE					hKey = NULL;
	BOOL					bReturn;
	PER_IO_CONTEXT*			pIoContext;


	while (1)
	{
		bReturn = GetQueuedCompletionStatus(pThis->m_hIOCompletionPort,
											&dwBytesTransfered,
											(ULONG*)&hKey,
											&pOverlapped,
											INFINITE);
		if ( EXIT_CODE==hKey)
		{
			break;
		}
		if(NULL == pOverlapped )  
		{  
			DWORD dwErr = GetLastError();
			continue;  
		} 
		else
		{			
			pIoContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, overlapped);
			if (dwBytesTransfered)		
			{
				pThis->PushFileNotifyInfo(pIoContext->pFileNotification);
				pThis->PostWatchFileChange(pIoContext);
			}
			else	//CloseDir
			{
				pIoContext->Clear();
			}
		}
	}
	return 0;
}

void CIOCPCheckDirChange::PushFileNotifyInfo(PFILE_NOTIFY_INFORMATION pFileNotify)
{
// 	m_listLock.Lock();
// 	m_notifyInfoList.push_back(pFileNotify);
// 	m_listLock.Unlock();

	m_notifyInfoList.PushBack(pFileNotify);
	
	ReleaseSemaphore(m_hSempEvent[0],1,NULL);
}

UINT __stdcall CIOCPCheckDirChange::AnalyDirChangeThread(LPVOID	lpParam)
{
	CIOCPCheckDirChange* pThis = (CIOCPCheckDirChange*) lpParam;
	DWORD	dwWait;
	while(1)
	{
		dwWait = WaitForMultipleObjects(2,pThis->m_hSempEvent,FALSE,INFINITE);
		if (WAIT_OBJECT_0 == dwWait)
		{
			PFILE_NOTIFY_INFORMATION& pNotify = pThis->m_notifyInfoList.Front();
			pThis->AnalyDirChangeEvent(pNotify);
			delete [] ((char*)pNotify);

// 			pThis->m_listLock.Lock();
// 			pThis->m_notifyInfoList.pop_front();
// 			pThis->m_listLock.Unlock();
			pThis->m_notifyInfoList.PopFront();
		}
		else if ((WAIT_OBJECT_0+1) == dwWait)
		{
			break;
		}
	}
	
	return 0;
}

bool CIOCPCheckDirChange::OpenDirectory()
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

void CIOCPCheckDirChange::CloseDirectory()
{
	if (m_hDirectory!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hDirectory);
		m_hDirectory = INVALID_HANDLE_VALUE;
	} 
}

void CIOCPCheckDirChange::AnalyDirChangeEvent(PFILE_NOTIFY_INFORMATION pFileNotify)
{
    DWORD cbOffset;
	
	do 
	{
		cbOffset = pFileNotify->NextEntryOffset;

		CheckAddedFile(  pFileNotify );

		pFileNotify = (PFILE_NOTIFY_INFORMATION)((LPBYTE) pFileNotify + cbOffset);

     } while( cbOffset );
}

void CIOCPCheckDirChange::CheckAddedFile(PFILE_NOTIFY_INFORMATION pFileNotify)
{
	memset(m_szFileName,0,MAX_PATH*sizeof(TCHAR));
	memcpy( m_szFileName, pFileNotify->FileName ,pFileNotify->FileNameLength);

	::WideCharToMultiByte(CP_ACP,0,(LPCWSTR)m_szFileName, pFileNotify->FileNameLength, m_csFileFullNames, MAX_PATH,NULL,NULL); 
	m_changeFile.AddItem(m_csFileFullNames,pFileNotify->Action);
}