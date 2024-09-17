#if !defined(AFX_SCRIPTCOMPRESSOR_H__4382AA60_F0A5_4E75_87FF_4D98214C2F10__INCLUDED_)
#define AFX_SCRIPTCOMPRESSOR_H__4382AA60_F0A5_4E75_87FF_4D98214C2F10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptCompressor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ScriptCompressor dialog

class ScriptCompressor : public CDialog
{
// Construction
public:
	ScriptCompressor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ScriptCompressor)
	enum { IDD = IDD_COMPRESS_SCRIPTS };
	BOOL	m_delete_c_files;
	BOOL	m_remove_comments;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ScriptCompressor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ScriptCompressor)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTCOMPRESSOR_H__4382AA60_F0A5_4E75_87FF_4D98214C2F10__INCLUDED_)
