#if !defined(AFX_DMODPROPERTIESDIALOG_H__F4C7EFFF_4C59_4AD1_B1FE_8129CD2C9E49__INCLUDED_)
#define AFX_DMODPROPERTIESDIALOG_H__F4C7EFFF_4C59_4AD1_B1FE_8129CD2C9E49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DmodPropertiesDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DmodPropertiesDialog dialog

class DmodPropertiesDialog : public CDialog
{
// Construction
public:
	DmodPropertiesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DmodPropertiesDialog)
	enum { IDD = IDD_DMOD_PROPERTIES };
	CString	m_author;
	CString	m_description;
	CString	m_dmod_title;
	CString	m_email_website;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DmodPropertiesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DmodPropertiesDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DMODPROPERTIESDIALOG_H__F4C7EFFF_4C59_4AD1_B1FE_8129CD2C9E49__INCLUDED_)
