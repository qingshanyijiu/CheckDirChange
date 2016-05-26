; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTestDirChangeDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TestDirChange.h"

ClassCount=3
Class1=CTestDirChangeApp
Class2=CTestDirChangeDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_TESTDIRCHANGE_DIALOG

[CLS:CTestDirChangeApp]
Type=0
HeaderFile=TestDirChange.h
ImplementationFile=TestDirChange.cpp
Filter=N

[CLS:CTestDirChangeDlg]
Type=0
HeaderFile=TestDirChangeDlg.h
ImplementationFile=TestDirChangeDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BUTTON_SelDir

[CLS:CAboutDlg]
Type=0
HeaderFile=TestDirChangeDlg.h
ImplementationFile=TestDirChangeDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_TESTDIRCHANGE_DIALOG]
Type=1
Class=CTestDirChangeDlg
ControlCount=8
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT_Path,edit,1350631552
Control3=IDC_BUTTON_SelDir,button,1342242816
Control4=IDC_BUTTON_Begin,button,1342242816
Control5=IDC_BUTTON_End,button,1342242816
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,button,1342177287
Control8=IDC_LIST_Show,SysListView32,1350631425

