// ScreenProperties.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "ScreenProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScreenProperties dialog


ScreenProperties::ScreenProperties(CWnd* pParent /*=NULL*/)
	: CDialog(ScreenProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(ScreenProperties)
	m_inside = FALSE;
	m_midi = 0;
	m_script = _T("");
	//}}AFX_DATA_INIT
}


void ScreenProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ScreenProperties)
	DDX_Check(pDX, IDC_Inside, m_inside);
	DDX_Text(pDX, IDC_Midi, m_midi);
	DDX_Text(pDX, IDC_Script, m_script);
	DDV_MaxChars(pDX, m_script, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ScreenProperties, CDialog)
	//{{AFX_MSG_MAP(ScreenProperties)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ScreenProperties message handlers

void ScreenProperties::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
