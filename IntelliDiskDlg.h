/* This file is part of IntelliDisk application developed by Mihai MOGA.

IntelliDisk is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliDisk is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliDisk.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// IntelliDiskDlg.h : header file
//

#pragma once

#include "FileInformation.h"
#include "NotifyDirCheck.h"
#include "WndResizer.h"

// CIntelliDiskDlg dialog
class CIntelliDiskDlg : public CDialogEx
{
// Construction
public:
	CIntelliDiskDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INTELLIDISK_DIALOG };
#endif
	CString	m_strFolder;
	CNotifyDirCheck m_pNotifyDirCheck;
	CWndResizer m_pWindowResizer;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangeEditDir();
	afx_msg void OnBrowse();
	afx_msg void OnStart();
	afx_msg void OnStop();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
public:
	void ShowEvent(CString event);
};
