// SpriteLibraryEntry.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "SpriteLibraryEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SpriteLibraryEntry dialog


SpriteLibraryEntry::SpriteLibraryEntry(CWnd* pParent /*=NULL*/)
	: CDialog(SpriteLibraryEntry::IDD, pParent)
{
	//{{AFX_DATA_INIT(SpriteLibraryEntry)
	m_sprite_name = _T("");
	//}}AFX_DATA_INIT
}


void SpriteLibraryEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SpriteLibraryEntry)
	DDX_Text(pDX, IDC_Name, m_sprite_name);
	DDV_MaxChars(pDX, m_sprite_name, 19);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SpriteLibraryEntry, CDialog)
	//{{AFX_MSG_MAP(SpriteLibraryEntry)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SpriteLibraryEntry message handlers
