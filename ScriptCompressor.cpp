// ScriptCompressor.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "ScriptCompressor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptCompressor dialog


ScriptCompressor::ScriptCompressor(CWnd* pParent /*=NULL*/)
	: CDialog(ScriptCompressor::IDD, pParent)
{
	//{{AFX_DATA_INIT(ScriptCompressor)
	m_delete_c_files = FALSE;
	m_remove_comments = FALSE;
	//}}AFX_DATA_INIT
}


void ScriptCompressor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScriptCompressor)
	DDX_Check(pDX, IDC_DELETE_C_FILES, m_delete_c_files);
	DDX_Check(pDX, IDC_REMOVE_COMMENTS, m_remove_comments);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ScriptCompressor, CDialog)
	//{{AFX_MSG_MAP(ScriptCompressor)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScriptCompressor message handlers
