#if !defined(AFX_VISIONCHANGE_H__672A6FA6_D602_11D4_9600_00A0C9A2392A__INCLUDED_)
#define AFX_VISIONCHANGE_H__672A6FA6_D602_11D4_9600_00A0C9A2392A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VisionChange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VisionChange dialog

class VisionChange : public CDialog
{
// Construction
public:
	VisionChange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(VisionChange)
	enum { IDD = IDD_VISION };
	int		m_vision;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VisionChange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VisionChange)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISIONCHANGE_H__672A6FA6_D602_11D4_9600_00A0C9A2392A__INCLUDED_)
