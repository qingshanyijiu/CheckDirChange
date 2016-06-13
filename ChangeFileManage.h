// ChangeFileManage.h: interface for the CChangeFileManage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGEFILEMANAGE_H__E65DDA3E_26F8_42E2_B56D_15555541EF95__INCLUDED_)
#define AFX_CHANGEFILEMANAGE_H__E65DDA3E_26F8_42E2_B56D_15555541EF95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CriticalLock.h"
#include <string>
#include <list>
using namespace std;

typedef struct tagFileInfo
{
	DWORD	dwAction;
	string  strFileName;

	tagFileInfo(DWORD dwaction,const char* lpName){
		dwAction = dwaction;
		strFileName = lpName;
	}

}FileInfo,*PFileInfo;

class CChangeFileManage  
{
public:
	CChangeFileManage();
	virtual ~CChangeFileManage();

	void Init();
	void Start();
	void AddItem(const char* lpFileName,DWORD dwAction);
	void Stop();

protected:
	static UINT	__stdcall	DisposeMsgInfo(LPVOID lpParam);
	void DisposeOneMsg();

protected:
	HANDLE				m_hThread;
	HANDLE				m_hSignal[2];
	list<FileInfo>		m_fileNameList;
	CCriticalLock		m_csLock;
	int					m_nFileNum;

};

#endif // !defined(AFX_CHANGEFILEMANAGE_H__E65DDA3E_26F8_42E2_B56D_15555541EF95__INCLUDED_)
