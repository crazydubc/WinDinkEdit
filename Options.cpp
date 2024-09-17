// Options.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "Options.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Options dialog


Options::Options(CWnd* pParent /*=NULL*/)
	: CDialog(Options::IDD, pParent)
	, m_show_progress(FALSE)
	, m_dink_ini(FALSE)
	, m_brush_size(0)
	, m_fast_minimap(FALSE)
	, m_hover_sprite_info(FALSE)
	, m_hover_sprite_hardness(FALSE)
	, m_help_text(FALSE)
{
	//{{AFX_DATA_INIT(Options)
	m_screen_gap = 0;
	m_max_undos = 0;
	m_autosave_time = 0;
	m_show_progress = true;
	//}}AFX_DATA_INIT
}


void Options::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Options)
	DDX_Text(pDX, IDC_SCREEN_GAP, m_screen_gap);
	DDV_MinMaxUInt(pDX, m_screen_gap, 0, 20);
	DDX_Text(pDX, IDC_MAX_UNDOS, m_max_undos);
	DDV_MinMaxUInt(pDX, m_max_undos, 0, 1000);
	DDX_Text(pDX, IDC_AUTO_SAVE, m_autosave_time);
	DDV_MinMaxUInt(pDX, m_autosave_time, 0, 20);
	DDX_Check(pDX, IDC_SHOW_PROGRESS, m_show_progress);
	DDX_Check(pDX, IDC_DINK_INI, m_dink_ini);
	DDX_Text(pDX, IDC_BRUSH_SIZE, m_brush_size);
	DDV_MinMaxInt(pDX, m_brush_size, 1, 20);
	DDX_Check(pDX, IDC_FAST_MINIMAP, m_fast_minimap);
	DDX_Check(pDX, IDC_HOVER_SPRITE_INFO, m_hover_sprite_info);
	DDX_Check(pDX, IDC_CHECK2, m_hover_sprite_hardness);
	DDX_Check(pDX, IDC_HELP_TEXT, m_help_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Options, CDialog)
	//{{AFX_MSG_MAP(Options)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Options message handlers
