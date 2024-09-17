// SpriteProperties.cpp : implementation file
//

#include "stdafx.h"
#include "WinDinkedit.h"
#include "SpriteProperties.h"
#include "Interface.h"
#include "Sprite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SpriteProperties dialog

void SpriteProperties::loadSpriteData(Sprite* cur_sprite)
{
	sprite_selected = cur_sprite;

	// fill in the information that is already available
	m_base_attack		= sprite_selected->base_attack;
	m_base_death		= sprite_selected->base_death;
	m_base_idle			= sprite_selected->base_idle;
	m_base_walk			= sprite_selected->base_walk;
	m_brain				= sprite_selected->brain;
	m_defense			= sprite_selected->defense;
	m_depth_que			= sprite_selected->depth;
	m_enable_warp		= sprite_selected->warp_enabled;
	m_frame				= sprite_selected->frame;
	m_hard				= !sprite_selected->hardness;
	m_hitpoints			= sprite_selected->hitpoints;
	m_nohit				= sprite_selected->nohit;
	m_sequence			= sprite_selected->sequence;
	m_sound				= sprite_selected->sound;
	m_speed				= sprite_selected->speed;
	m_timing			= sprite_selected->timing;
	m_touch_damage		= sprite_selected->touch_damage;
	m_touch_sequence	= sprite_selected->touch_sequence;
	m_vision			= sprite_selected->getVision();
	m_warp_screen		= sprite_selected->warp_screen;
	m_warp_x			= sprite_selected->warp_x;
	m_warp_y			= sprite_selected->warp_y;
	m_x					= sprite_selected->x;
	m_y					= sprite_selected->y;
	m_size				= sprite_selected->size;
	m_type				= sprite_selected->type;
	m_experience		= sprite_selected->experience;
	m_script			= sprite_selected->script;	
}

void SpriteProperties::saveSpriteData()
{
	sprite_selected->base_attack		= m_base_attack;
	sprite_selected->base_death			= m_base_death;
	sprite_selected->base_idle			= m_base_idle;
	sprite_selected->base_walk			= m_base_walk;
	sprite_selected->brain				= m_brain;
	sprite_selected->defense			= m_defense;
	sprite_selected->depth				= m_depth_que;
	sprite_selected->warp_enabled		= m_enable_warp;
	sprite_selected->frame				= m_frame;
	sprite_selected->hardness			= !m_hard;
	sprite_selected->hitpoints			= m_hitpoints;
	sprite_selected->nohit				= m_nohit;
	sprite_selected->sequence			= m_sequence;
	sprite_selected->sound				= m_sound;
	sprite_selected->speed				= m_speed;
	sprite_selected->timing				= m_timing;
	sprite_selected->touch_damage		= m_touch_damage;
	sprite_selected->touch_sequence		= m_touch_sequence;
	sprite_selected->setVision(m_vision);
	sprite_selected->warp_screen		= m_warp_screen;
	sprite_selected->warp_x				= m_warp_x;
	sprite_selected->warp_y				= m_warp_y;
	sprite_selected->x					= m_x;
	sprite_selected->y					= m_y;
	sprite_selected->size				= m_size;
	sprite_selected->type				= m_type;
	sprite_selected->experience			= m_experience;
	strcpy(sprite_selected->script, m_script.operator LPCTSTR());

	// sets the world vision to the sprite's new vision
	//setVision(m_vision);
}

SpriteProperties::SpriteProperties(CWnd* pParent /*=NULL*/)
	: CDialog(SpriteProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(SpriteProperties)
	m_x = 0;
	m_y = 0;
	m_vision = 0;
	m_touch_damage = 0;
	m_script = _T("");
	m_nohit = FALSE;
	m_hard = FALSE;
	m_enable_warp = FALSE;
	m_base_walk = 0;
	m_base_death = 0;
	m_base_attack = 0;
	m_depth_que = 0;
	m_base_idle = 0;
	m_type = -1;
	m_experience = 0;
	m_defense = 0;
	m_hitpoints = 0;
	m_sequence = 0;
	m_size = 0;
	m_sound = 0;
	m_speed = 0;
	m_frame = 0;
	m_timing = 0;
	m_touch_sequence = 0;
	m_warp_screen = 0;
	m_warp_x = 0;
	m_warp_y = 0;
	m_brain = -1;
	//}}AFX_DATA_INIT
}


void SpriteProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SpriteProperties)
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_Vision, m_vision);
	DDX_Text(pDX, IDC_Touch_damage, m_touch_damage);
	DDX_Text(pDX, IDC_Script, m_script);
	DDV_MaxChars(pDX, m_script, 13);
	DDX_Check(pDX, IDC_Nohit, m_nohit);
	DDX_Check(pDX, IDC_Hard, m_hard);
	DDX_Check(pDX, IDC_Enable_warp, m_enable_warp);
	DDX_Text(pDX, IDC_Base_walk, m_base_walk);
	DDV_MinMaxInt(pDX, m_base_walk, -1, 1000);
	DDX_Text(pDX, IDC_Base_death, m_base_death);
	DDV_MinMaxInt(pDX, m_base_death, -1, 1000);
	DDX_Text(pDX, IDC_Base_attack, m_base_attack);
	DDV_MinMaxInt(pDX, m_base_attack, -1, 1000);
	DDX_Text(pDX, IDC_Depth_que, m_depth_que);
	DDX_Text(pDX, IDC_Base_idle, m_base_idle);
	DDV_MinMaxInt(pDX, m_base_idle, -1, 1000);
	DDX_Radio(pDX, IDC_ORNAMENTAL, m_type);
	DDX_Text(pDX, IDC_Experience_Given, m_experience);
	DDX_Text(pDX, IDC_Defense, m_defense);
	DDX_Text(pDX, IDC_Hitpoints, m_hitpoints);
	DDX_Text(pDX, IDC_Sequence, m_sequence);
	DDV_MinMaxUInt(pDX, m_sequence, 1, 1000);
	DDX_Text(pDX, IDC_Size, m_size);
	DDX_Text(pDX, IDC_Sound, m_sound);
	DDX_Text(pDX, IDC_Speed, m_speed);
	DDX_Text(pDX, IDC_Frame, m_frame);
	DDV_MinMaxUInt(pDX, m_frame, 1, 50);
	DDX_Text(pDX, IDC_Timing, m_timing);
	DDX_Text(pDX, IDC_Touch_sequence, m_touch_sequence);
	DDX_Text(pDX, IDC_Warp_screen, m_warp_screen);
	DDV_MinMaxUInt(pDX, m_warp_screen, 0, 768);
	DDX_Text(pDX, IDC_Warp_x, m_warp_x);
	DDX_Text(pDX, IDC_Warp_y, m_warp_y);
	DDX_CBIndex(pDX, IDC_BRAIN, m_brain);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SpriteProperties, CDialog)
	//{{AFX_MSG_MAP(SpriteProperties)
	ON_BN_CLICKED(IDC_EDIT_SPRITE_SCRIPT, OnEditSpriteScript)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SpriteProperties message handlers


// open up the script in the default text editor
void SpriteProperties::OnEditSpriteScript() 
{
	UpdateData();

	if (m_script.GetLength() > 0)
		editScript(m_script);
}
