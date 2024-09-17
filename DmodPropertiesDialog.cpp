// DmodPropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "DmodPropertiesDialog.h"
#include "Common.h"
#include "Map.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DmodPropertiesDialog dialog


DmodPropertiesDialog::DmodPropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DmodPropertiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(DmodPropertiesDialog)
	m_author = _T("");
	m_description = _T("");
	m_dmod_title = _T("");
	m_email_website = _T("");
	//}}AFX_DATA_INIT

	readDmodDiz(current_map->dmod_path.GetBuffer(0), m_dmod_title, m_author, m_email_website, m_description);
}


void DmodPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DmodPropertiesDialog)
	DDX_Text(pDX, IDC_AUTHOR, m_author);
	DDX_Text(pDX, IDC_DMOD_DESCRIPTION, m_description);
	DDX_Text(pDX, IDC_DMOD_TITLE, m_dmod_title);
	DDX_Text(pDX, IDC_EMAIL_WEBSITE, m_email_website);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DmodPropertiesDialog, CDialog)
	//{{AFX_MSG_MAP(DmodPropertiesDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DmodPropertiesDialog message handlers

void DmodPropertiesDialog::OnOK() 
{
	UpdateData();
	
	writeDmodDiz(current_map->dmod_path.GetBuffer(0), m_dmod_title.GetBuffer(0), 
		m_author.GetBuffer(0), m_email_website.GetBuffer(0), m_description.GetBuffer(0));
	
	CDialog::OnOK();
}
