// WinDinkedit.h : main header file for the WINDINKEDIT application
//

#ifndef WINDINKEDIT_H
#define WINDINKEDIT_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditApp:
// See WinDinkedit.cpp for the implementation of this class
//

class CWinDinkeditApp : public CWinApp
{
public:
	CWinDinkeditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinDinkeditApp)
	public:
	virtual BOOL InitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual void OnFileOpen();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWinDinkeditApp)
	afx_msg void OnAppAbout();
	afx_msg void OnNewDmod();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
