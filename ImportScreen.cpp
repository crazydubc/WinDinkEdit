// ImportScreen.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "ImportScreen.h"
#include "Globals.h"
#include "Map.h"
#include "ImportMap.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ImportScreen dialog


ImportScreen::ImportScreen(CWnd* pParent /*=NULL*/)
	: CDialog(ImportScreen::IDD, pParent)
{
	//{{AFX_DATA_INIT(ImportScreen)
	//}}AFX_DATA_INIT

	
}


void ImportScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ImportScreen)
	DDX_Control(pDX, IDC_DMOD_IMPORT_LIST, m_dmod_list);
	//}}AFX_DATA_MAP
}

void ImportScreen::makeDmodList()
{
	createDmodList(main_dink_path.GetBuffer(0), &m_dmod_list);

	if (current_map->import_dmod_path.GetLength() == 0)
	{
		m_dmod_list.SelectString(-1, getDmodName(current_map->dmod_path) );
	}
	else
	{
		m_dmod_list.SelectString(-1, getDmodName(current_map->import_dmod_path));
	}
}


BEGIN_MESSAGE_MAP(ImportScreen, CDialog)
	//{{AFX_MSG_MAP(ImportScreen)
	ON_LBN_DBLCLK(IDC_DMOD_IMPORT_LIST, OnDblclkDmodImportList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ImportScreen message handlers

BOOL ImportScreen::OnInitDialog() 
{
	CDialog::OnInitDialog();

	makeDmodList();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// load the dmod import screen
void ImportScreen::OnOK()
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

	current_map->import_dmod_path = main_dink_path + dmod_name_entered + CString("\\");


	current_map->render_state = SHOW_SCREEN_IMPORTER;

	if (current_map->import_map == NULL)
		current_map->import_map = new ImportMap();
	
	CDialog::OnOK();
}

void ImportScreen::OnDblclkDmodImportList() 
{
	OnOK();
}
