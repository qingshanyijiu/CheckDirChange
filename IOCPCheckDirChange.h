// CheckDirChange.h: interface for the CCheckDirChange class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKDIRCHANGE_H__2F84F82F_A635_4A40_8F7C_F6B5999D36B0__INCLUDED_)
#define AFX_CHECKDIRCHANGE_H__2F84F82F_A635_4A40_8F7C_F6B5999D36B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChangeFileManage.h"
#include "CriticalLock.h"
//#include <list>
#include "FastList.h"
using namespace std;

#define		MAX_FILE_NOTIFY_LEN		(8192)


typedef struct tagPER_IO_CONTEXT
{
	OVERLAPPED					overlapped;
	char*						pNotifyBuffer;
	FILE_NOTIFY_INFORMATION *	pFileNotification;
	DWORD						dwReturnLen;
	
	tagPER_IO_CONTEXT()
	{
		memset(&overlapped,0,sizeof(OVERLAPPED));
		pNotifyBuffer = NULL;
		pFileNotification = NULL;
		dwReturnLen = 0;
	}

	void GetNewFileNotifyBuffer()
	{
		pNotifyBuffer = new char[MAX_FILE_NOTIFY_LEN];
		memset(pNotifyBuffer,0,MAX_FILE_NOTIFY_LEN);
		pFileNotification = (FILE_NOTIFY_INFORMATION*)pNotifyBuffer;
		dwReturnLen = 0;
	}

	
	void Clear()
	{
		if (pNotifyBuffer)
			delete [] pNotifyBuffer;

		pNotifyBuffer = NULL;
	}
	
}PER_IO_CONTEXT,*PPER_IO_CONTEXT;


class CIOCPCheckDirChange  
{
public:
	CIOCPCheckDirChange();
	virtual ~CIOCPCheckDirChange();

	void Init();
	bool StartCheck(const char* lpDirPath);
	void StopCheck();

protected:
	void InitIOCP();
	void InitDispose();
	int	 GetNoOfProcessors();
	bool PostWatchFileChange(PPER_IO_CONTEXT pContext);

protected:
	static UINT __stdcall NotifiDirChangeThread(LPVOID	lpParam);
	static UINT __stdcall AnalyDirChangeThread(LPVOID	lpParam);
	void UnInit();
	bool OpenDirectory();
	void CloseDirectory();
	void AnalyDirChangeEvent(PFILE_NOTIFY_INFORMATION pFileNotify);
	void CheckAddedFile(PFILE_NOTIFY_INFORMATION pFileNotify);
	void PushFileNotifyInfo(PFILE_NOTIFY_INFORMATION pFileNotify);

protected:
	HANDLE							m_hIOCompletionPort;           // 完成端口的句柄
	HANDLE							m_hDirectory;
	HANDLE*							m_phWorkThreads;
	PPER_IO_CONTEXT					m_pContexts;
	HANDLE							m_hDisposeThread;
	HANDLE							m_hSempEvent[2];
	char							m_csWatchFilePath[MAX_PATH];
	int								m_nWorkerCount;
	int								m_nMaxContextsCount;

protected:
	CFastList<PFILE_NOTIFY_INFORMATION>	m_notifyInfoList;
	//CCriticalLock					m_listLock;
	CChangeFileManage				m_changeFile;

protected:
	char							m_csFileFullNames[MAX_PATH];
	TCHAR							m_szFileName[MAX_PATH];
};

#endif // !defined(AFX_CHECKDIRCHANGE_H__2F84F82F_A635_4A40_8F7C_F6B5999D36B0__INCLUDED_)
