#if !defined(AFX_FASTFILE_H__D2D3EB37_3ABD_41D6_883E_307F21AEDF42__INCLUDED_)
#define AFX_FASTFILE_H__D2D3EB37_3ABD_41D6_883E_307F21AEDF42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Fastfile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Fastfile dialog

class Fastfile : public CDialog
{
// Construction
public:
	Fastfile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Fastfile)
	enum { IDD = IDD_FFCREATE };
	BOOL	m_delete_graphics;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Fastfile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Fastfile)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTFILE_H__D2D3EB37_3ABD_41D6_883E_307F21AEDF42__INCLUDED_)
