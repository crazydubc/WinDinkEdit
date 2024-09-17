// WinDinkeditView.cpp : implementation of the CWinDinkeditView class
//

#include "stdafx.h"
#include "WinDinkedit.h"

#include "WinDinkeditDoc.h"
#include "WinDinkeditView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Engine.h"
#include "Globals.h"
#include "Map.h"
#include "Screen.h"
#include "SpriteProperties.h"
#include "ScreenProperties.h"
#include "VisionChange.h"
#include "ImportScreen.h"
#include "Interface.h"
#include "Structs.h"
#include "Minimap.h"
#include "SpriteLibrary.h"
#include "SpriteLibraryEntry.h"
#include "Fastfile.h"
#include "Tools.h"
#include "ScriptCompressor.h"
#include "Options.h"
#include "Sprite.h"

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditView

IMPLEMENT_DYNCREATE(CWinDinkeditView, CView)

BEGIN_MESSAGE_MAP(CWinDinkeditView, CView)
	//{{AFX_MSG_MAP(CWinDinkeditView)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
	ON_WM_MOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_COMMAND(ID_SCREEN_PROPERTIES, OnScreenProperties)
	ON_COMMAND(ID_DELETE_SCREEN, OnDeleteScreen)
	ON_COMMAND(IDM_SPRITE_PROPERTIES, OnSpriteProperties)
	ON_COMMAND(IDM_VISION, OnVision)
	ON_COMMAND(IDM_NEW_SCREEN, OnNewScreen)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_VIEW_MINIMAP, OnViewMinimap)
	ON_COMMAND(IDM_VIEW_SPRITE_MODE, OnViewSpriteMode)
	ON_COMMAND(IDM_VIEW_TILE_MODE, OnViewTileMode)
	ON_COMMAND(IDM_VIEW_TILESET_1, OnViewTileset1)
	ON_COMMAND(IDM_VIEW_TILESET_10, OnViewTileset10)
	ON_COMMAND(IDM_VIEW_TILESET_11, OnViewTileset11)
	ON_COMMAND(IDM_VIEW_TILESET_3, OnViewTileset3)
	ON_COMMAND(IDM_VIEW_TILESET_4, OnViewTileset4)
	ON_COMMAND(IDM_VIEW_TILESET_5, OnViewTileset5)
	ON_COMMAND(IDM_VIEW_TILESET_6, OnViewTileset6)
	ON_COMMAND(IDM_VIEW_TILESET_7, OnViewTileset7)
	ON_COMMAND(IDM_VIEW_TILESET_8, OnViewTileset8)
	ON_COMMAND(IDM_VIEW_TILESET_9, OnViewTileset9)
	ON_COMMAND(IDM_VIEW_TILESET_2, OnViewTileset2)
	ON_COMMAND(IDM_SCREEN_MATCH, OnScreenMatch)
	ON_WM_PAINT()
	ON_COMMAND(IDM_SPRITE_HARDNESS, OnSpriteHardness)
	ON_COMMAND(IDM_VIEW_HARDBOX_MODE, OnViewHardboxMode)
	ON_COMMAND(IDM_SET_WARP_BEGIN, OnSetWarpBegin)
	ON_COMMAND(IDM_SET_WARP_END, OnSetWarpEnd)
	ON_COMMAND(IDM_IMPORT_SCREEN, OnImportScreen)
	ON_COMMAND(IDM_AUTO_UPDATE_MINIMAP, OnAutoUpdateMinimap)
	ON_COMMAND(IDM_SCREEN_SHOT, OnScreenShot)
	ON_COMMAND(IDM_STORE_SPRITE, OnStoreSprite)
	ON_COMMAND(IDM_FFCREATE, OnFfcreate)
	ON_COMMAND(IDM_COPY_SCREEN, OnCopyScreen)
	ON_COMMAND(IDM_PASTE_SCREEN, OnPasteScreen)
	ON_COMMAND(IDM_COMPRESS_SCRIPTS, OnCompressScripts)
	ON_COMMAND(IDM_OPTIONS, OnOptions)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(IDM_REVERT_HARD_TILE, OnRevertHardTile)
	ON_COMMAND(IDM_TRANSFORM_HARDNESS_NORMAL, OnTransformHardnessNormal)
	ON_COMMAND(IDM_TRANSFORM_HARDNESS_LOW, OnTransformHardnessLow)
	ON_COMMAND(IDM_TRANSFORM_HARDNESS_UNKNOWN, OnTransformHardnessUnknown)
	ON_COMMAND(IDM_SET_DEFAULT_HARD_TILE, OnSetDefaultHardTile)
	ON_COMMAND(IDM_EDIT_HARDNESS, OnEditHardTile)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(IDM_AUTO_UPDATE_MINIMAP, OnAutoUpdateMinimap)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditView construction/destruction

CWinDinkeditView::CWinDinkeditView()
{
	// TODO: add construction code here
}

CWinDinkeditView::~CWinDinkeditView()
{
	if (current_map)
	{
		delete current_map;
		current_map = NULL;
	}
}

BOOL CWinDinkeditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditView drawing

void CWinDinkeditView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

//	view_window = this;
	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditView diagnostics

#ifdef _DEBUG
void CWinDinkeditView::AssertValid() const
{
	CView::AssertValid();
}

void CWinDinkeditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinDinkeditDoc* CWinDinkeditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinDinkeditDoc)));
	return (CWinDinkeditDoc*)m_pDocument;
}
#endif //_DEBUG

void CWinDinkeditView::gainFocus()
{
	SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
// CWinDinkeditView message handlers
void CWinDinkeditView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: add code to react to the user changing the view style of your window
}

// MOUSE COMMANDS ///////////////////////////////////////////////

void CWinDinkeditView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mouseLeftPressed(point.x, point.y);
	
	CView::OnLButtonDown(nFlags, point);
}

void CWinDinkeditView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mouseLeftReleased(point.x, point.y);

	CView::OnLButtonUp(nFlags, point);
}

void CWinDinkeditView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mouseLeftDoubleClick(point.x, point.y);
	
	CView::OnLButtonDblClk(nFlags, point);
}

void CWinDinkeditView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mouseRightPressed(point.x, point.y, this);
	CView::OnRButtonDown(nFlags, point);
}

void CWinDinkeditView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mouseMove(point.x, point.y);
	
	CView::OnMouseMove(nFlags, point);
}

// KEYBOARD COMMANDS ////////////////////////////////////////////////

void CWinDinkeditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	keyPressed((UCHAR)nChar);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CWinDinkeditView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	keyReleased((UCHAR)nChar);

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

// WINDOW CHANGES //////////////////////////////////////////////////

void CWinDinkeditView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	GetWindowRect(&mapRect);
	need_resizing = true;
}

void CWinDinkeditView::OnMove(int x, int y) 
{
	CView::OnMove(x, y);
	
	// TODO: Add your message handler code here
	GetWindowRect(&mapRect);
}

// called from mainframe
int CWinDinkeditView::windowMoved(int x, int y)
{
	GetWindowRect(&mapRect);

	return true;
}

// context menus
Sprite* sprite_clicked = NULL;
int screen_num_clicked = 0;
int sprite_num_clicked = 0;
Sprite* begin_warp = NULL;

void CWinDinkeditView::displaySpriteMenu(Sprite* sprite, int screen_num, int sprite_num, int x, int y)
{
	sprite_clicked = sprite;
	screen_num_clicked = screen_num;
	sprite_num_clicked = sprite_num;

	CMenu menuPopup;
	menuPopup.LoadMenu(IDR_SPRITE_MENU);

	//only allow if possiible to set warp end.
	if (current_map->getMouseSprite() == NULL && begin_warp == NULL)
	{
		//now, load the sub menu into the pointer
		CMenu *submenu;
		submenu = menuPopup.GetSubMenu(0);

		//now, gray out the menu for 'set warp end'.
		submenu->EnableMenuItem(IDM_SET_WARP_END,MF_BYCOMMAND|MF_GRAYED);
	}

	//don't allow begin if we have a mousesprite
	if (current_map->getMouseSprite())
	{
		//now, load the sub menu into the pointer
		CMenu *submenu;
		submenu = menuPopup.GetSubMenu(0);

		//now, gray out the menu for 'set warp begin'.
		submenu->EnableMenuItem(IDM_SET_WARP_BEGIN,MF_BYCOMMAND|MF_GRAYED);
	}

	menuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, x + mapRect.left, y + mapRect.top, this);
}

void CWinDinkeditView::displayScreenMenu(int screen, int x, int y)
{
	screen_num_clicked = screen;

	if (current_map->screen[screen_num_clicked] == NULL)
	{
		CMenu menuPopup;
		menuPopup.LoadMenu(IDR_NEW_SCREEN);
		menuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, x + mapRect.left, y + mapRect.top, this);
	}
	else
	{
		//use diffrent menu if in hardbox mode
		if (current_map->screenMode == HARDBOX_MODE)
		{
			//click the mouse so we only have one tile selected.
			mouseLeftPressed(x,y);
			mouseLeftReleased(x,y);

			//load the hard box menu into CMenu Varible
			CMenu menuPopup;
			menuPopup.LoadMenu(IDR_HARD_MENU);

			//lets check if the tile can be reverted. If not, don't display the menu

			//get the current tile
			int x_tile = ((x + current_map->window.left) % (SCREEN_WIDTH + screen_gap)) / TILEWIDTH;
			int y_tile = ((y + current_map->window.top) % (SCREEN_HEIGHT + screen_gap)) / TILEHEIGHT;

			//if there isn't a alternate hardness, it can't be reverted to default.
			if (current_map->screen[screen_num_clicked]->tiles[y_tile][x_tile].alt_hardness == 0)
			{
				//now, load the sub menu into the pointer
				CMenu *submenu;
				submenu = menuPopup.GetSubMenu(0);

				//now, gray out the menu for 'revert hard tile'.
				submenu->EnableMenuItem(IDM_REVERT_HARD_TILE,MF_BYCOMMAND|MF_GRAYED);
			}
			
			//only allow if possiible to set warp end.
			if (begin_warp == NULL || current_map->screenMode != SPRITE_MODE)
			{
				//now, load the sub menu into the pointer
				CMenu *submenu;
				submenu = menuPopup.GetSubMenu(0);

				//now, gray out the menu for 'set warp end'.
				submenu->EnableMenuItem(IDM_SET_WARP_END,MF_BYCOMMAND|MF_GRAYED);
			}

			//display menu and track what the user clicks.
			menuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, x + mapRect.left, y + mapRect.top, this);
		} else {
			CMenu menuPopup;
			menuPopup.LoadMenu(IDR_SCREEN_MENU);

			//only allow if possiible to set warp end.
			if (begin_warp == NULL || current_map->screenMode != SPRITE_MODE)
			{
				//now, load the sub menu into the pointer
				CMenu *submenu;
				submenu = menuPopup.GetSubMenu(0);

				//now, gray out the menu for 'set warp end'.
				submenu->EnableMenuItem(IDM_SET_WARP_END,MF_BYCOMMAND|MF_GRAYED);
			}
			menuPopup.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, x + mapRect.left, y + mapRect.top, this);
		}
	}
}

void CWinDinkeditView::OnSpriteProperties() 
{
	// create the dialog box
	SpriteProperties newdlg(this);

	// fill in the information that is already available
	Sprite *temp_sprite = new Sprite;
	memcpy(temp_sprite, sprite_clicked, sizeof(Sprite));
	newdlg.loadSpriteData(temp_sprite);

	// now show the dialog box
	int retcode = newdlg.DoModal();

	// check if ok or cancel was pressed
	if (retcode == IDOK)
	{
		// ok was pressed, copy the data back over
		newdlg.saveSpriteData();

		//if (sprite_clicked == current_map->mouseSprite)
		//	temp_sprite->x = current_map->x_hover + SIDEBAR_WIDTH;

		current_map->changeSpriteProperties(screen_num_clicked, sprite_num_clicked, temp_sprite, sprite_clicked);
		current_map->miniupdated[screen_num_clicked] = false;
	}
	else
	{
		delete temp_sprite;
	}
}

void CWinDinkeditView::OnVision() 
{
	// create the dialog box
	VisionChange newdlg(this);
	newdlg.m_vision = current_map->cur_vision;

	// now show the dialog box
	int retcode = newdlg.DoModal();

	// check if ok or cancel was pressed
	if (retcode == IDOK)
	{
		setVision(newdlg.m_vision);
	}
}

void CWinDinkeditView::OnStoreSprite() 
{
	// create the dialog box
	SpriteLibraryEntry newdlg(this);

	// now show the dialog box
	int retcode = newdlg.DoModal();

	// check if ok or cancel was pressed
	if (retcode == IDOK)
	{
		char buffer[MAX_SPRITE_NAME_LENGTH];
		strcpy(buffer, newdlg.m_sprite_name.operator LPCTSTR());

		current_map->sprite_library.addSprite(sprite_clicked, buffer);
	}
}

void CWinDinkeditView::OnImportScreen() 
{
	if (current_map->import_map == NULL)
	{
		ImportScreen newdlg(this);

		int retcode = newdlg.DoModal();
	}
}

void CWinDinkeditView::OnViewMinimap() 
{
	keyPressed('\t');
	keyReleased('\t');
}

void CWinDinkeditView::OnViewSpriteMode() 
{
	keyPressed('r');
	keyReleased('r');
}

void CWinDinkeditView::OnViewTileMode() 
{
	keyPressed('t');
	keyReleased('t');
}

void CWinDinkeditView::OnViewHardboxMode() 
{
	keyPressed('h');
	keyReleased('h');
}

void CWinDinkeditView::OnViewTileset1() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('1');
		keyReleased('1');	
	}
}

void CWinDinkeditView::OnViewTileset2() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('2');
		keyReleased('2');
	}
}

void CWinDinkeditView::OnViewTileset3() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('3');
		keyReleased('3');
	}
}

void CWinDinkeditView::OnViewTileset4() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('4');
		keyReleased('4');
	}
}

void CWinDinkeditView::OnViewTileset5() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('5');
		keyReleased('5');	
	}
}

void CWinDinkeditView::OnViewTileset6() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('6');
		keyReleased('6');
	}
}

void CWinDinkeditView::OnViewTileset7() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('7');
		keyReleased('7');	
	}
}

void CWinDinkeditView::OnViewTileset8() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('8');
		keyReleased('8');
	}
}

void CWinDinkeditView::OnViewTileset9() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('9');
		keyReleased('9');
	}
}

void CWinDinkeditView::OnViewTileset10() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed('0');
		keyReleased('0');
	}
}

void CWinDinkeditView::OnViewTileset11() 
{
	if(current_map->screenMode == TILE_MODE && current_map->render_state != SHOW_TILE_HARDNESS_EDITOR)
	{
		keyPressed(KEY_BACKQUOTE);
		keyReleased(KEY_BACKQUOTE);
	}
}

void CWinDinkeditView::OnScreenMatch() 
{
	keyPressed('m');
	keyReleased('m');	
}

void CWinDinkeditView::OnSpriteHardness() 
{
	keyPressed('d');
	keyReleased('d');	
}

void CWinDinkeditView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	GetWindowRect(&mapRect);
	Game_Main();
}

void CWinDinkeditView::OnSetWarpBegin() 
{
	begin_warp = sprite_clicked;
}

void CWinDinkeditView::OnSetWarpEnd() 
{
	if (begin_warp != NULL)
	{
		if (current_map->screen[screen_num_clicked] != NULL)
		{
			begin_warp->warp_screen = screen_num_clicked + 1;
			begin_warp->warp_x = current_map->x_hover + 20;
			begin_warp->warp_y = current_map->y_hover;
			begin_warp->warp_enabled = true;
			begin_warp->hardness = 0;
		}
	}	
}


BOOL CWinDinkeditView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{
	if (sizeScroll.cy > 0)
	{
		keyPressed(KEY_ARROW_DOWN);
		keyReleased(KEY_ARROW_DOWN);
	}

	if (sizeScroll.cy < 0)
	{
		keyPressed(KEY_ARROW_UP);
		keyReleased(KEY_ARROW_UP);
	}
	
	return CView::OnScrollBy(sizeScroll, bDoScroll);
}

BOOL CWinDinkeditView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll) 
{
	// TODO: Add your specialized code here and/or call the base class 
	
	return CView::OnScroll(SB_LINEDOWN | SB_LINEUP , nPos, TRUE);
}

void CWinDinkeditView::OnAutoUpdateMinimap() 
{
	current_map->toggleAutoUpdateMinimap();
}

void CWinDinkeditView::OnAutoUpdateMinimap(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();

	pCmdUI->SetCheck(update_minimap);
}


#include <fstream.h>

void CWinDinkeditView::OnScreenShot() 
{
	char filename[250];

	//our file name will be Screen1.bmp to Screen999.bmp
	//this for loop will make it so we can have all 999 .bmp's if neccissarry
	for (int i = 1; i < 1000; i++)
	{
		//copy it all over to the file name
		wsprintf(filename, "%sScreen%.3d.bmp", current_map->dmod_path, i);
		
		//try to open the file but don't create a file. If it fails, then
		//there is no image, and we will use this file! If not, continue
		ofstream fout(filename, ios::noreplace);
		if (fout)
		{
			//send the file name over so we can get a screen shot!
			current_map->screenshot(filename);
			return; //return when done so we don't create any more screen shots.
		}
	}
	//uh oh, the user used all the files. Lets tell them.
	MessageBox("Too many Screen shots. Please Delete some and try again.", NULL, NULL);
}

void CWinDinkeditView::OnFfcreate() 
{
	// create the dialog box
	Fastfile newdlg(this);

	// now show the dialog box
	int retcode = newdlg.DoModal();

	// check if ok or cancel was pressed
	if (retcode == IDOK)
	{
		bool delete_graphics = bool(newdlg.m_delete_graphics);

		char buffer[250];
		wsprintf(buffer, "%sGraphics\\", current_map->dmod_path);

		ffcreate(buffer, delete_graphics);
	}

//	MessageBox("This feature has been disabled due to incompatibilites", "Disabled", MB_OK);
}

void CWinDinkeditView::OnCompressScripts() 
{
	// create the dialog box
	ScriptCompressor newdlg(this);

	// now show the dialog box
	int retcode = newdlg.DoModal();

	// check if ok or cancel was pressed
	if (retcode == IDOK)
	{
		compressAll((BOOL)newdlg.m_delete_c_files, (BOOL)newdlg.m_remove_comments);
	}	
}

void CWinDinkeditView::OnNewScreen() 
{
	current_map->newScreen(screen_num_clicked);
}

void CWinDinkeditView::OnDeleteScreen() 
{
	current_map->deleteScreen(screen_num_clicked);
}

void CWinDinkeditView::OnScreenProperties() 
{
	if (current_map->screen[screen_num_clicked] == NULL)
		return;

	// create the dialog box
	ScreenProperties newdlg(this);
	newdlg.m_script = current_map->screen[screen_num_clicked]->script;
	newdlg.m_midi = current_map->midi_num[screen_num_clicked];
	newdlg.m_inside = current_map->indoor[screen_num_clicked];

	// now show the dialog box
	int retcode = newdlg.DoModal();

	// check if ok or cancel was pressed
	if (retcode == IDOK)
	{
		current_map->changeScreenProperties(screen_num_clicked, 
			newdlg.m_script.operator LPCTSTR(), newdlg.m_midi, newdlg.m_inside);
	}
}

int copy_screen = 0;

void CWinDinkeditView::OnCopyScreen() 
{
	copy_screen = screen_num_clicked;
}

void CWinDinkeditView::OnPasteScreen() 
{
	if (current_map->screen[copy_screen] != NULL)
	{
		current_map->pasteScreen(screen_num_clicked, current_map->screen[copy_screen],
			current_map->midi_num[copy_screen], current_map->indoor[copy_screen]);
	}
}

void CWinDinkeditView::OnOptions() 
{
	Options newdlg(this);

	newdlg.m_screen_gap = screen_gap;
	newdlg.m_max_undos = max_undo_level;
	newdlg.m_autosave_time = auto_save_time;
	newdlg.m_show_progress = show_minimap_progress;
	newdlg.m_dink_ini = optimize_dink_ini;
	newdlg.m_brush_size = tile_brush_size;
	newdlg.m_fast_minimap = fast_minimap_update;
	newdlg.m_hover_sprite_info = hover_sprite_info;
	newdlg.m_hover_sprite_hardness = hover_sprite_info;
	newdlg.m_help_text = help_text;

	// now show the dialog box
	int retcode = newdlg.DoModal();

	// check if ok or cancel was pressed
	if (retcode == IDOK)
	{
		screen_gap = newdlg.m_screen_gap;
		max_undo_level = newdlg.m_max_undos;
		auto_save_time = newdlg.m_autosave_time;
		show_minimap_progress = newdlg.m_show_progress;
		tile_brush_size = newdlg.m_brush_size;
		hover_sprite_info = newdlg.m_hover_sprite_info;
		hover_sprite_hardness = newdlg.m_hover_sprite_hardness;
		help_text = newdlg.m_help_text;

		if(update_minimap && !newdlg.m_fast_minimap && fast_minimap_update)
		{
			int retcode = MessageBox("Note: Disabling Fast Minimap Update can slow the program while update minimap is on, do you wish to continue?", 
				"Alert: Optimization Change", MB_YESNO);

			if (retcode == IDYES)
			{
				fast_minimap_update = newdlg.m_fast_minimap;
			}

		}else
		{
			fast_minimap_update = newdlg.m_fast_minimap;
		}

		if ((!optimize_dink_ini) && newdlg.m_dink_ini)
		{
			int retcode = MessageBox("Note: Optimize Dink Ini feature will remove all comments, redundant commands, and sort you Dink.ini file for fast loading. Do you wish to continue?", 
				"Alert: Optimization Change", MB_YESNO);

			if (retcode == IDYES)
			{
				optimize_dink_ini = newdlg.m_dink_ini;
			}
		}
		else
		{
			optimize_dink_ini = newdlg.m_dink_ini;
		}
		KillTimer(TIMER_AUTO_SAVE);

		if (auto_save_time)
		{
			SetTimer(TIMER_AUTO_SAVE, auto_save_time * 60000, NULL);
		}
	}
	Game_Main();
}

// undo an action if possible
void CWinDinkeditView::OnEditUndo() 
{
	if (current_map->render_state == SHOW_SCREEN || current_map->render_state == SHOW_MINIMAP)
	{
		current_map->undoAction();
	}
}

// redo an action if possible
void CWinDinkeditView::OnEditRedo() 
{
	if (current_map->render_state == SHOW_SCREEN || current_map->render_state == SHOW_MINIMAP)
	{
		current_map->redoAction();
	}
}

void CWinDinkeditView::OnRevertHardTile()
{
	current_map->hard_tile_selector.revertTile();
}

void CWinDinkeditView::OnTransformHardnessNormal()
{
	current_map->hard_tile_selector.TransformHardTile(1);
}

void CWinDinkeditView::OnTransformHardnessLow()
{
	current_map->hard_tile_selector.TransformHardTile(2);
}

void CWinDinkeditView::OnTransformHardnessUnknown()
{
	current_map->hard_tile_selector.TransformHardTile(3);
}

void CWinDinkeditView::OnSetDefaultHardTile()
{
	current_map->hard_tile_selector.SetDefaultHardTile();
}

void CWinDinkeditView::OnEditHardTile()
{
	keyPressed('q');
}
