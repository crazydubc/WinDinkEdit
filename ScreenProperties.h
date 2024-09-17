#if !defined(AFX_SCREENPROPERTIES_H__672A6FA4_D602_11D4_9600_00A0C9A2392A__INCLUDED_)
#define AFX_SCREENPROPERTIES_H__672A6FA4_D602_11D4_9600_00A0C9A2392A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScreenProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScreenProperties dialog

class ScreenProperties : public CDialog
{
// Construction
public:
	ScreenProperties(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ScreenProperties)
	enum { IDD = IDD_SCREEN_PROPERTIES };
	BOOL	m_inside;
	int		m_midi;
	CString	m_script;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScreenProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScreenProperties)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREENPROPERTIES_H__672A6FA4_D602_11D4_9600_00A0C9A2392A__INCLUDED_)
