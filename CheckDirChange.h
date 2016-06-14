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
#include <list>
using namespace std;

typedef struct tagReadDirChangeInfo
{
	DWORD						dwMaxBufferLen;
	DWORD						dwReturnLen;
	FILE_NOTIFY_INFORMATION*	pFileNotification;
	OVERLAPPED					overlapped;
}ReadDirChangeInfo,*PReadDirChangeInfo;

class CCheckDirChange  
{
public:
	CCheckDirChange();
	virtual ~CCheckDirChange();

	void Init();
	bool StartCheck(const char* lpDirPath);
	void StopCheck();

protected:
	static UINT __stdcall NotifiDirChangeThread(LPVOID	lpParam);
	static UINT __stdcall AnalyDirChangeThread(LPVOID	lpParam);
	void UnInit();
	bool OpenDirectory();
	void CloseDirectory();
	void AnalyDirChangeEvent(PFILE_NOTIFY_INFORMATION pFileNotify);
	void CheckAddedFile(PFILE_NOTIFY_INFORMATION pFileNotify);
	bool WatchDirChanges(PReadDirChangeInfo pChangeInfo);
	bool WatchDirChanges(PReadDirChangeInfo pChangeInfos[],int length);
	void PushFileNotifyInfo(PFILE_NOTIFY_INFORMATION pFileNotify);
	void ChangeEventAndChangeInfo(PReadDirChangeInfo pChangeInfo[],HANDLE hWatchEvent[],int lenght,int FireEventIndexs[],int FireEventNum);

protected:
	HANDLE							m_hDirectory;
	HANDLE							m_hThread[2];
	HANDLE							m_hSempEvent[2];
	HANDLE							m_hExitWatchEvent;
	HANDLE							m_hBeginWatchEvent;
	char							m_csWatchFilePath[MAX_PATH];

protected:
	list<PFILE_NOTIFY_INFORMATION>	m_notifyInfoList;
	CCriticalLock					m_listLock;
	CChangeFileManage				m_changeFile;

protected:
	char							m_csFileFullNames[MAX_PATH];
	TCHAR							m_szFileName[MAX_PATH];
};

#endif // !defined(AFX_CHECKDIRCHANGE_H__2F84F82F_A635_4A40_8F7C_F6B5999D36B0__INCLUDED_)
