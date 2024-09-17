#if !defined(AFX_IMPORTSCREEN_H__908101F2_0DF7_42C2_B9F8_DB9AC96610D7__INCLUDED_)
#define AFX_IMPORTSCREEN_H__908101F2_0DF7_42C2_B9F8_DB9AC96610D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportScreen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ImportScreen dialog

class ImportScreen : public CDialog
{
public:
	void makeDmodList();

// Construction
public:
	ImportScreen(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ImportScreen)
	enum { IDD = IDD_IMPORT_SCREEN };
	CListBox	m_dmod_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ImportScreen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ImportScreen)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkDmodImportList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTSCREEN_H__908101F2_0DF7_42C2_B9F8_DB9AC96610D7__INCLUDED_)
