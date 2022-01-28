/* This file is part of IntelliDisk application developed by Mihai MOGA.

IntelliDisk is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliDisk is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliDisk.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// IntelliDiskDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "IntelliDisk.h"
#include "IntelliDiskDlg.h"

#include "VersionInfo.h"
#include "HyperlinkStatic.h"
#include "NTray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_TRAYNOTIFY WM_USER + 100
#pragma warning(suppress: 26426)
CTrayNotifyIcon m_TrayIcon;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

protected:
	CFont m_fontCourier;
	CStatic m_ctrlVersion;
	CEdit m_ctrlWarning;
	CVersionInfo m_pVersionInfo;
	CHyperlinkStatic m_ctrlWebsite;
	CHyperlinkStatic m_ctrlEmail;

	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION, m_ctrlVersion);
	DDX_Control(pDX, IDC_WARNING, m_ctrlWarning);
	DDX_Control(pDX, IDC_WEBSITE, m_ctrlWebsite);
	DDX_Control(pDX, IDC_EMAIL, m_ctrlEmail);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	TCHAR lpszDrive[_MAX_DRIVE];
	TCHAR lpszDirectory[_MAX_DIR];
	TCHAR lpszFilename[_MAX_FNAME];
	TCHAR lpszExtension[_MAX_EXT];
	TCHAR lpszFullPath[_MAX_PATH];

	VERIFY(0 == _tsplitpath_s(AfxGetApp()->m_pszHelpFilePath, lpszDrive, _MAX_DRIVE, lpszDirectory, _MAX_DIR, lpszFilename, _MAX_FNAME, lpszExtension, _MAX_EXT));
	VERIFY(0 == _tmakepath_s(lpszFullPath, _MAX_PATH, lpszDrive, lpszDirectory, lpszFilename, _T(".exe")));

	if (m_pVersionInfo.Load(lpszFullPath))
	{
		CString strName = m_pVersionInfo.GetProductName().c_str();
		CString strVersion = m_pVersionInfo.GetProductVersionAsString().c_str();
		strVersion.Replace(_T(" "), _T(""));
		strVersion.Replace(_T(","), _T("."));
		const int nFirst = strVersion.Find(_T('.'));
		const int nSecond = strVersion.Find(_T('.'), nFirst + 1);
		strVersion.Truncate(nSecond);
		m_ctrlVersion.SetWindowText(strName + _T(" version ") + strVersion);
	}

	// set up window's terminal fixed font
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	logfont.lfCharSet = OEM_CHARSET;
	logfont.lfPitchAndFamily = FIXED_PITCH;
	// logfont.lfQuality = ANTIALIASED_QUALITY;
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfWeight = FW_NORMAL;
	logfont.lfHeight = -MulDiv(6, GetDeviceCaps(::GetDC(NULL), LOGPIXELSY), 72);;
	_tcscpy_s(logfont.lfFaceName, LF_FACESIZE, _T("Tahoma"));
	VERIFY(m_fontCourier.CreateFontIndirect(&logfont));

	m_ctrlWarning.SetFont(&m_fontCourier);
	m_ctrlWarning.SetWindowText(_T("THE SOFTWARE IS PROVIDED \"AS-IS\" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL STEFAN-MIHAI MOGA BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE."));

	m_ctrlWebsite.SetHyperlink(_T("https://www.intellidisk.site/"));
	m_ctrlEmail.SetHyperlink(_T("mailto:contact@emvs.site"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	VERIFY(m_fontCourier.DeleteObject());
}

// CIntelliDiskDlg dialog

CIntelliDiskDlg::CIntelliDiskDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INTELLIDISK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIntelliDiskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_EVENT, m_strFolder);
	DDV_MaxChars(pDX, m_strFolder, 1024);
}

BEGIN_MESSAGE_MAP(CIntelliDiskDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_TRAYNOTIFY, OnTrayNotification)
	ON_EN_CHANGE(IDC_STATIC_EVENT, OnChangeEditDir)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STOP, OnStop)
END_MESSAGE_MAP()

// CIntelliDiskDlg message handlers

BOOL CIntelliDiskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	VERIFY(m_pWindowResizer.Hook(this));

	VERIFY(m_pWindowResizer.SetAnchor(IDC_STOP, ANCHOR_RIGHT | ANCHOR_BOTTOM));
	VERIFY(m_pWindowResizer.SetAnchor(IDC_START, ANCHOR_RIGHT | ANCHOR_BOTTOM));
	VERIFY(m_pWindowResizer.SetAnchor(IDC_BROWSE, ANCHOR_RIGHT | ANCHOR_BOTTOM));
	VERIFY(m_pWindowResizer.SetAnchor(IDC_STATIC_EVENT, ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_BOTTOM));
	VERIFY(m_pWindowResizer.SetAnchor(IDC_STATUS, ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_TOP | ANCHOR_BOTTOM));

	// TODO: Add extra initialization here
	if (!m_TrayIcon.Create(this, IDR_POPUP_MENU, _T("IntelliDisk"), m_hIcon, WM_TRAYNOTIFY))
	{
		ATLTRACE(_T("Failed to create tray icon\n"));
		return FALSE;
	}

	/* Set windows size zero, the windows disappear.
	MoveWindow(0, 0, 0, 0);

	// If you want it invisible on taskbar. add following codes.
	DWORD dwStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
	dwStyle &= ~WS_EX_APPWINDOW;
	dwStyle |= WS_EX_TOOLWINDOW;
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, dwStyle);*/

	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_EVENT)->EnableWindow(TRUE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIntelliDiskDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIntelliDiskDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIntelliDiskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CIntelliDiskDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	// Delegate all the work back to the default implementation in CTrayNotifyIcon.
	m_TrayIcon.OnTrayNotification(wParam, lParam);
	return 0L;
}

void CIntelliDiskDlg::OnChangeEditDir()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_START)->EnableWindow(!m_strFolder.IsEmpty());
}

UINT DirCallback(CFileInformation fiObject, EFileAction faAction, LPVOID lpData)
{
	CString           csBuffer;
	CString           csFile = fiObject.GetFilePath();
	CIntelliDiskDlg* pDlg = (CIntelliDiskDlg*)lpData;

	if (IS_CREATE_FILE(faAction))
	{
		csBuffer.Format(_T("Created %s"), csFile);
	}
	else if (IS_DELETE_FILE(faAction))
	{
		csBuffer.Format(_T("Deleted %s"), csFile);
	}
	else if (IS_CHANGE_FILE(faAction))
	{
		csBuffer.Format(_T("Changed %s"), csFile);
	}
	else
	{
		return 1; //error, stop thread
	}

	pDlg->ShowEvent(csBuffer);

	return 0; //success
}

void CIntelliDiskDlg::OnStart()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_EVENT)->EnableWindow(FALSE);

	m_pNotifyDirCheck.SetDirectory(m_strFolder);
	m_pNotifyDirCheck.SetData(this);

	// set your callback to work with each new event
	m_pNotifyDirCheck.SetActionCallback(DirCallback);
	// set NULL callback value and override Action to work with each new event
	// m_pNotifyDirCheck.SetActionCallback( NULL );

	m_pNotifyDirCheck.Run();
}

void CIntelliDiskDlg::OnStop()
{
	m_pNotifyDirCheck.Stop();

	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_EVENT)->EnableWindow(TRUE);
}

void CIntelliDiskDlg::OnCancel()
{
	OnStop();
	CDialogEx::OnCancel();
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	TCHAR szDir[MAX_PATH];

	switch (uMsg)
	{
		case BFFM_INITIALIZED:
		{
			if (GetCurrentDirectory(sizeof(szDir) / sizeof(TCHAR), szDir))
				SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)szDir);
			break;
		}
		case BFFM_SELCHANGED:
		{
			if (SHGetPathFromIDList((LPITEMIDLIST)lp, szDir))
				SendMessage(hwnd, BFFM_SETSTATUSTEXT, (WPARAM)NULL, (LPARAM)szDir);
		}
		default:
		{
			break;
		}
	}

	return 0;
}

void CIntelliDiskDlg::OnBrowse()
{
	BROWSEINFO   bi;
	TCHAR        szDir[MAX_PATH];
	LPITEMIDLIST pidl;
	LPMALLOC     pMalloc;

	UpdateData(TRUE);

	if (SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		ZeroMemory(&bi, sizeof(bi));
		bi.hwndOwner = m_hWnd;
		bi.lpszTitle = _T("Directory Location...");
		bi.pszDisplayName = 0;
		bi.pidlRoot = 0;
		bi.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
		bi.lpfn = BrowseCallbackProc;

		pidl = SHBrowseForFolder(&bi);

		if (pidl)
		{
			if (SHGetPathFromIDList(pidl, szDir))
			{
				m_strFolder = szDir;
				UpdateData(FALSE);
			}

			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}
}

void CIntelliDiskDlg::ShowEvent(CString event)
{
	GetDlgItem(IDC_STATIC_EVENT)->SetWindowText(event);
}
