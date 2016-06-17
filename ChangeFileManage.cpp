// ChangeFileManage.cpp: implementation of the CChangeFileManage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestIOCPDirChange.h"
#include "ChangeFileManage.h"
#include "TestIOCPDirChangeDlg.h"
#include <process.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_FILE_COUNT		(0xFFFFFFF)

CChangeFileManage::CChangeFileManage()
{
	m_hThread = NULL;
	m_hSignal[0] = NULL;
	m_hSignal[1] = NULL;
	m_nFileNum = 0;
}

CChangeFileManage::~CChangeFileManage()
{
	Stop();
}


void CChangeFileManage::Init()
{
	if (NULL == m_hThread)
	{
		m_nFileNum = 0;
		m_hSignal[0] = ::CreateEvent(NULL,FALSE,FALSE,NULL);
		m_hSignal[1] = ::CreateSemaphore(NULL,0,MAX_FILE_COUNT,NULL);
		m_hThread	 = (HANDLE)_beginthreadex(NULL,0,DisposeMsgInfo,this,0,NULL);
	}
}

void CChangeFileManage::Start()
{
	m_nFileNum = 0;
}

void CChangeFileManage::AddItem(const char* lpFileName,DWORD dwAction)
{
	{
		CCriticalLock::CAutoLock	lock(m_csLock);
		m_fileNameList.push_back(FileInfo(dwAction,lpFileName));
	}

	ReleaseSemaphore(m_hSignal[1],1,NULL);
}

void CChangeFileManage::Stop()
{
	if (m_hThread)
	{
		SetEvent(m_hSignal[0]);
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread,2000))
		{
			TerminateThread(m_hThread,0);
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;

		CloseHandle(m_hSignal[0]);
		CloseHandle(m_hSignal[1]);
	}

	m_fileNameList.clear();
}

UINT __stdcall	CChangeFileManage::DisposeMsgInfo(LPVOID lpParam)
{
	CChangeFileManage* pThis = (CChangeFileManage*) lpParam;
	DWORD	dwWait;

	while(1)
	{
		dwWait	= WaitForMultipleObjects(2,pThis->m_hSignal,FALSE,INFINITE);
		if ((WAIT_OBJECT_0+1) == dwWait)
		{
			pThis->DisposeOneMsg();
		}
		else if (WAIT_OBJECT_0 == dwWait)
		{
			break;
		}
	}

	return 0;
}

void CChangeFileManage::DisposeOneMsg()
{
	FileInfo&	fileInfo = m_fileNameList.front();

	++m_nFileNum;
	CTestIOCPDirChangeDlg::InsertItem(m_nFileNum,&fileInfo);

	CCriticalLock::CAutoLock lock(m_csLock);
	m_fileNameList.pop_front();
}