#if !defined(AFX_NEWDMODDIALOG_H__87836A94_9B55_44BF_ABB1_08195F61C8A6__INCLUDED_)
#define AFX_NEWDMODDIALOG_H__87836A94_9B55_44BF_ABB1_08195F61C8A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewDmodDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NewDmodDialog dialog

class NewDmodDialog : public CDialog
{
// Construction
public:
	NewDmodDialog(CWnd* pParent = NULL);   // standard constructor
	void setDmodNamePtr(char *buffer);

// Dialog Data
	//{{AFX_DATA(NewDmodDialog)
	enum { IDD = IDD_NEW_DMOD };
	CString	m_description;
	CString	m_directory;
	CString	m_dmod_name;
	CString	m_author;
	CString	m_email_website;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewDmodDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(NewDmodDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	char *dmod_name;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWDMODDIALOG_H__87836A94_9B55_44BF_ABB1_08195F61C8A6__INCLUDED_)
