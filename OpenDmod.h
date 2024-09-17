#if !defined(AFX_OPENDMOD_H__468158F3_D584_47CB_8548_986C2DAEAD1C__INCLUDED_)
#define AFX_OPENDMOD_H__468158F3_D584_47CB_8548_986C2DAEAD1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenDmod.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OpenDmod dialog

class OpenDmod : public CDialog
{
// Construction
public:
	OpenDmod(CWnd* pParent = NULL);   // standard constructor
	void setDmodNamePtr(char *buffer);

// Dialog Data
	//{{AFX_DATA(OpenDmod)
	enum { IDD = IDD_OPEN_DMOD };
	CListBox	m_dmod_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OpenDmod)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OpenDmod)
	virtual void OnOK();
	afx_msg void OnDblclkDmodList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	char *dmod_selected;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENDMOD_H__468158F3_D584_47CB_8548_986C2DAEAD1C__INCLUDED_)
