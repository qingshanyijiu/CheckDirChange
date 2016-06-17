// TestIOCPDirChange.h : main header file for the TestIOCPDirChange application
//

#if !defined(AFX_TestIOCPDirChange_H__911D8578_C594_4275_8B2E_237E65C6E60C__INCLUDED_)
#define AFX_TestIOCPDirChange_H__911D8578_C594_4275_8B2E_237E65C6E60C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestIOCPDirChangeApp:
// See TestIOCPDirChange.cpp for the implementation of this class
//

class CTestIOCPDirChangeApp : public CWinApp
{
public:
	CTestIOCPDirChangeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestIOCPDirChangeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestIOCPDirChangeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TestIOCPDirChange_H__911D8578_C594_4275_8B2E_237E65C6E60C__INCLUDED_)
