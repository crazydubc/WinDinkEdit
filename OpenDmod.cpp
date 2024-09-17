// OpenDmod.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "OpenDmod.h"
#include "Common.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OpenDmod dialog


OpenDmod::OpenDmod(CWnd* pParent /*=NULL*/)
	: CDialog(OpenDmod::IDD, NULL)
{
	//{{AFX_DATA_INIT(OpenDmod)
	//}}AFX_DATA_INIT
}


void OpenDmod::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OpenDmod)
	DDX_Control(pDX, IDC_DMOD_LIST, m_dmod_list);
	//}}AFX_DATA_MAP

	createDmodList(main_dink_path.GetBuffer(0), &m_dmod_list);
}


BEGIN_MESSAGE_MAP(OpenDmod, CDialog)
	//{{AFX_MSG_MAP(OpenDmod)
	ON_LBN_DBLCLK(IDC_DMOD_LIST, OnDblclkDmodList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OpenDmod message handlers

void OpenDmod::setDmodNamePtr(char *buffer)
{
	dmod_selected = buffer;
}

// open the dmod
void OpenDmod::OnOK()
{
	int index;
	
	// return if an invalid dmod name is selected
	if ((index = m_dmod_list.GetCurSel()) == CB_ERR)
	{
		MessageBox("Please select a dmod from the list", "Error", MB_OK);
		return;
	}

	CString dmod_name_entered;
	m_dmod_list.GetText(index, dmod_name_entered);
	
	strcpy(dmod_selected, dmod_name_entered);

	// TODO: Add extra validation here
	CDialog::OnOK();
}

// open the dmod
void OpenDmod::OnDblclkDmodList() 
{
	OnOK();
}
