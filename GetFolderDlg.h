// GetFolderDlg.h : header file
//

#if !defined(AFX_GETFOLDERDLG_H__4E404D5F_5906_460B_9E7D_BFC3355A69A3__INCLUDED_)
#define AFX_GETFOLDERDLG_H__4E404D5F_5906_460B_9E7D_BFC3355A69A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGetFolderDlg dialog

class CGetFolderDlg : public CDialog
{
// Construction
public:
	CGetFolderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGetFolderDlg)
	enum { IDD = IDD_GETFOLDER_DIALOG };
	CString	m_strFolderPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetFolderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGetFolderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGetfolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BOOL GetFolder(CString* strSelectedFolder, const char* lpszTitle, const HWND hwndOwner, const char* strRootFolder, const char* strStartFolder);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETFOLDERDLG_H__4E404D5F_5906_460B_9E7D_BFC3355A69A3__INCLUDED_)
