#include "afxwin.h"
#if !defined(AFX_OPTIONS_H__1D08AEDA_D1C6_4EE0_9745_C564C3F20867__INCLUDED_)
#define AFX_OPTIONS_H__1D08AEDA_D1C6_4EE0_9745_C564C3F20867__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Options.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Options dialog

class Options : public CDialog
{
// Construction
public:
	Options(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Options)
	enum { IDD = IDD_OPTIONS };
	UINT	m_screen_gap;
	UINT	m_max_undos;
	UINT    m_autosave_time;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Options)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Options)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_show_progress;
	BOOL m_dink_ini;
	int m_brush_size;
	BOOL m_fast_minimap;
	BOOL m_hover_sprite_info;
	BOOL m_hover_sprite_hardness;
	BOOL m_help_text;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONS_H__1D08AEDA_D1C6_4EE0_9745_C564C3F20867__INCLUDED_)
