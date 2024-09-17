#if !defined(AFX_SPRITELIBRARYENTRY_H__AC44F688_B3E7_43D4_9873_A4385EDDC437__INCLUDED_)
#define AFX_SPRITELIBRARYENTRY_H__AC44F688_B3E7_43D4_9873_A4385EDDC437__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpriteLibraryEntry.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SpriteLibraryEntry dialog

class SpriteLibraryEntry : public CDialog
{
// Construction
public:
	SpriteLibraryEntry(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SpriteLibraryEntry)
	enum { IDD = IDD_SPRITE_LIBRARY };
	CString	m_sprite_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SpriteLibraryEntry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SpriteLibraryEntry)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPRITELIBRARYENTRY_H__AC44F688_B3E7_43D4_9873_A4385EDDC437__INCLUDED_)
