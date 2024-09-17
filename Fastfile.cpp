// Fastfile.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "Fastfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Fastfile dialog


Fastfile::Fastfile(CWnd* pParent /*=NULL*/)
	: CDialog(Fastfile::IDD, pParent)
{
	//{{AFX_DATA_INIT(Fastfile)
	m_delete_graphics = FALSE;
	//}}AFX_DATA_INIT
}


void Fastfile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Fastfile)
	DDX_Check(pDX, IDC_DELETE_GRAPHICS, m_delete_graphics);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Fastfile, CDialog)
	//{{AFX_MSG_MAP(Fastfile)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Fastfile message handlers
