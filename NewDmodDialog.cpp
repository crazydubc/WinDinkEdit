// NewDmodDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "NewDmodDialog.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NewDmodDialog dialog


NewDmodDialog::NewDmodDialog(CWnd* pParent /*=NULL*/)
	: CDialog(NewDmodDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(NewDmodDialog)
	m_description = _T("");
	m_directory = _T("");
	m_dmod_name = _T("");
	m_author = _T("");
	m_email_website = _T("");
	//}}AFX_DATA_INIT
}


void NewDmodDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NewDmodDialog)
	DDX_Text(pDX, IDC_DMOD_DESCRIPTION, m_description);
	DDX_Text(pDX, IDC_DMOD_DIRECTORY_NAME, m_directory);
	DDX_Text(pDX, IDC_DMOD_TITLE, m_dmod_name);
	DDX_Text(pDX, IDC_AUTHOR, m_author);
	DDX_Text(pDX, IDC_EMAIL_WEBSITE, m_email_website);
	//}}AFX_DATA_MAP
}

void NewDmodDialog::setDmodNamePtr(char *buffer)
{
	dmod_name = buffer;
}


BEGIN_MESSAGE_MAP(NewDmodDialog, CDialog)
	//{{AFX_MSG_MAP(NewDmodDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NewDmodDialog message handlers

void NewDmodDialog::OnOK() 
{
	UpdateData();
	
	if (strlen(m_directory.GetBuffer(0)) == 0)
	{
		MessageBox("You must specify a dmod directory name before continuing", "Must specify name", MB_OK);
		return;
	}
	
	if (createDmod(m_directory.GetBuffer(0), m_dmod_name.GetBuffer(0), 
		m_author.GetBuffer(0), m_email_website.GetBuffer(0), m_description.GetBuffer(0)) == false)
	{
		CDialog::OnCancel();
	}
	else
	{
		strcpy(dmod_name, m_directory.GetBuffer(0));
		CDialog::OnOK();
	}
}
