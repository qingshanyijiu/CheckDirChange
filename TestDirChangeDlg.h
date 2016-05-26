// TestDirChangeDlg.h : header file
//

#if !defined(AFX_TESTDIRCHANGEDLG_H__0C3E5E7E_CDF1_4D1E_AE4F_415FA719FA16__INCLUDED_)
#define AFX_TESTDIRCHANGEDLG_H__0C3E5E7E_CDF1_4D1E_AE4F_415FA719FA16__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CheckDirChange.h"

/////////////////////////////////////////////////////////////////////////////
// CTestDirChangeDlg dialog

class CTestDirChangeDlg : public CDialog
{
// Construction
public:
	CTestDirChangeDlg(CWnd* pParent = NULL);	// standard constructor

public:
	static	void InsertItem(int iNum,const char* lpName);
	
protected:
	void AddItem(int iNum,const char* lpName);
	void ClearItem();
	
protected:
	CCheckDirChange m_dirChange;

// Dialog Data
	//{{AFX_DATA(CTestDirChangeDlg)
	enum { IDD = IDD_TESTDIRCHANGE_DIALOG };
	CListCtrl	m_showList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDirChangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestDirChangeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBUTTONBegin();
	afx_msg void OnBUTTONEnd();
	afx_msg void OnBUTTONSelDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDIRCHANGEDLG_H__0C3E5E7E_CDF1_4D1E_AE4F_415FA719FA16__INCLUDED_)
