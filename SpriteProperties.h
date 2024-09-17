#if !defined(AFX_SPRITEPROPERTIES_H__A1309542_D3A4_11D4_9600_00A0C9A2392A__INCLUDED_)
#define AFX_SPRITEPROPERTIES_H__A1309542_D3A4_11D4_9600_00A0C9A2392A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpriteProperties.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SpriteProperties dialog

class Sprite;

class SpriteProperties : public CDialog
{
// Construction
public:
	SpriteProperties(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SpriteProperties)
	enum { IDD = IDD_SPRITE_PROPERTIES };
	int		m_x;
	int		m_y;
	int		m_vision;
	int		m_touch_damage;
	CString	m_script;
	BOOL	m_nohit;
	BOOL	m_hard;
	BOOL	m_enable_warp;
	int		m_base_walk;
	int		m_base_death;
	int		m_base_attack;
	int		m_depth_que;
	int		m_base_idle;
	int		m_type;
	UINT	m_experience;
	UINT	m_defense;
	UINT	m_hitpoints;
	UINT	m_sequence;
	UINT	m_size;
	UINT	m_sound;
	UINT	m_speed;
	UINT	m_frame;
	UINT	m_timing;
	UINT	m_touch_sequence;
	UINT	m_warp_screen;
	UINT	m_warp_x;
	UINT	m_warp_y;
	int		m_brain;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SpriteProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void loadSpriteData(Sprite* cur_sprite);
	void saveSpriteData();

private:
	Sprite* sprite_selected;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SpriteProperties)
	afx_msg void OnEditSpriteScript();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPRITEPROPERTIES_H__A1309542_D3A4_11D4_9600_00A0C9A2392A__INCLUDED_)
