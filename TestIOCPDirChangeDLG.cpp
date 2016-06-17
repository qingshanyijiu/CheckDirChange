// TestIOCPDirChangeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestIOCPDirChange.h"
#include "TestIOCPDirChangeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CTestIOCPDirChangeDlg*	g_pThis=NULL;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestIOCPDirChangeDlg dialog

CTestIOCPDirChangeDlg::CTestIOCPDirChangeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestIOCPDirChangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestIOCPDirChangeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	g_pThis = this;
}

void CTestIOCPDirChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestIOCPDirChangeDlg)
	DDX_Control(pDX, IDC_LIST_Show, m_showList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestIOCPDirChangeDlg, CDialog)
	//{{AFX_MSG_MAP(CTestIOCPDirChangeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Begin, OnBUTTONBegin)
	ON_BN_CLICKED(IDC_BUTTON_End, OnBUTTONEnd)
	ON_BN_CLICKED(IDC_BUTTON_SelDir, OnBUTTONSelDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestIOCPDirChangeDlg message handlers

BOOL CTestIOCPDirChangeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	LONG lStyle;
	lStyle = GetWindowLong(m_showList.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK; //清除显示方式位
	lStyle |= LVS_REPORT; //设置style
	SetWindowLong(m_showList.m_hWnd, GWL_STYLE, lStyle);//设置style
	DWORD dwStyle = m_showList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	m_showList.SetExtendedStyle(dwStyle); //设置扩展风格

	m_showList.InsertColumn( 0, "Num", LVCFMT_LEFT, 50 );
	m_showList.InsertColumn( 1, "Name", LVCFMT_LEFT, 100 );
	m_showList.InsertColumn( 2, "Note", LVCFMT_LEFT, 300 );
	SetDlgItemText(IDC_EDIT_Path,"D:\\TestCheck");

	m_dirChange.Init();

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestIOCPDirChangeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestIOCPDirChangeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestIOCPDirChangeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestIOCPDirChangeDlg::OnBUTTONBegin() 
{
	// TODO: Add your control notification handler code here

	CString strPath;
	GetDlgItemText(IDC_EDIT_Path,strPath);

	ClearItem();
	m_dirChange.StartCheck(strPath.operator LPCTSTR());

}

void CTestIOCPDirChangeDlg::OnBUTTONEnd() 
{
	// TODO: Add your control notification handler code here
	m_dirChange.StopCheck();
}

void CTestIOCPDirChangeDlg::OnBUTTONSelDir() 
{
	// TODO: Add your control notification handler code here
	CString sFolderPath;
	BROWSEINFO bi;
	char Buffer[MAX_PATH];
	//初始化入口参数bi开始
	bi.hwndOwner = NULL;
	
	LPITEMIDLIST pidl = NULL;
	bi.pidlRoot = pidl;//初始化制定的root目录很不容易，
	bi.pszDisplayName = Buffer;//此参数如为NULL则不能显示对话框
	bi.lpszTitle = "图片存放路径:";
	//bi.ulFlags = BIF_BROWSEINCLUDEFILES;//包括文件
	//bi.ulFlags = BIF_EDITBOX;//浏览对话框中包含一个编辑控件，用户可以键入项目名字。
	bi.ulFlags = BIF_RETURNONLYFSDIRS;//只返回文件夹，否则确定为灰。
	
	bi.lpfn = NULL;
	bi.iImage=IDR_MAINFRAME;
	//初始化入口参数bi结束
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if(pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);
		//取得文件夹路径到Buffer里
		sFolderPath = Buffer;//将路径保存在一个CString对象里
	}
	LPMALLOC lpMalloc;
	if(FAILED(SHGetMalloc(&lpMalloc))) return;
	//释放内存
	lpMalloc->Free(pIDList);
	lpMalloc->Release();
	
	SetDlgItemText(IDC_EDIT_Path,sFolderPath);
}


void CTestIOCPDirChangeDlg::InsertItem(int iNum,PFileInfo pInfo)
{
	g_pThis->AddItem(iNum,pInfo);
}

void CTestIOCPDirChangeDlg::AddItem(int iNum,PFileInfo pInfo)
{
	char csTemp[10] = {0};

	sprintf(csTemp,"%d",iNum);
	int nRow = m_showList.InsertItem(m_showList.GetItemCount(),csTemp);//插入行;
	m_showList.SetItemText(nRow,1,pInfo->strFileName.c_str());
	switch(pInfo->dwAction)
	{
	case FILE_ACTION_ADDED:
		m_showList.SetItemText(nRow,2,"File Added");
		break;
	case FILE_ACTION_REMOVED:
		m_showList.SetItemText(nRow,2,"File Removed");
		break;
	case FILE_ACTION_MODIFIED: 
		m_showList.SetItemText(nRow,2,"File Modified");
		break;
	case FILE_ACTION_RENAMED_OLD_NAME:
		m_showList.SetItemText(nRow,2,"File Rename old");
		break;
	case FILE_ACTION_RENAMED_NEW_NAME:
		m_showList.SetItemText(nRow,2,"File Rename new");
		break;
	}
}

void CTestIOCPDirChangeDlg::ClearItem()
{
	m_showList.DeleteAllItems();
}