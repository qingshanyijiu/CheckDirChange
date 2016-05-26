// TestDirChange.h : main header file for the TESTDIRCHANGE application
//

#if !defined(AFX_TESTDIRCHANGE_H__911D8578_C594_4275_8B2E_237E65C6E60C__INCLUDED_)
#define AFX_TESTDIRCHANGE_H__911D8578_C594_4275_8B2E_237E65C6E60C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestDirChangeApp:
// See TestDirChange.cpp for the implementation of this class
//

class CTestDirChangeApp : public CWinApp
{
public:
	CTestDirChangeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDirChangeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestDirChangeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDIRCHANGE_H__911D8578_C594_4275_8B2E_237E65C6E60C__INCLUDED_)
