// VisionChange.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "VisionChange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VisionChange dialog


VisionChange::VisionChange(CWnd* pParent /*=NULL*/)
	: CDialog(VisionChange::IDD, pParent)
{
	//{{AFX_DATA_INIT(VisionChange)
	m_vision = 0;
	//}}AFX_DATA_INIT
}


void VisionChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VisionChange)
	DDX_Text(pDX, IDC_Vision, m_vision);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VisionChange, CDialog)
	//{{AFX_MSG_MAP(VisionChange)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VisionChange message handlers

