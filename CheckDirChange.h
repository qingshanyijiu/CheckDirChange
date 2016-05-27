// CheckDirChange.h: interface for the CCheckDirChange class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKDIRCHANGE_H__2F84F82F_A635_4A40_8F7C_F6B5999D36B0__INCLUDED_)
#define AFX_CHECKDIRCHANGE_H__2F84F82F_A635_4A40_8F7C_F6B5999D36B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChangeFileManage.h"

class CCheckDirChange  
{
public:
	CCheckDirChange();
	virtual ~CCheckDirChange();

	void Init(const char* lpDirPath);
	void StopCheck();

protected:
	static UINT __stdcall NotifiDirChangeThread(LPVOID	lpParam);
	bool OpenDirectory(const char* lpDirPath);
	void AnalyDirChangeEvent(PFILE_NOTIFY_INFORMATION pFileNotify);
	void CheckAddedFile(PFILE_NOTIFY_INFORMATION pFileNotify);

protected:
	HANDLE					m_hDirectory;
	HANDLE					m_hThread;
	HANDLE					m_hExitEvent;
	CChangeFileManage		m_changeFile;
	char					m_filePath[MAX_PATH];
	
protected:
	char				m_csFileFullNames[MAX_PATH];
	TCHAR				m_szFileName[MAX_PATH];
};

#endif // !defined(AFX_CHECKDIRCHANGE_H__2F84F82F_A635_4A40_8F7C_F6B5999D36B0__INCLUDED_)
