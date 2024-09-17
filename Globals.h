#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <ddraw.h>
#include "GraphicsManager.h"

//#define new DEBUG_NEW

// DEFINES ////////////////////////////////////////////////

// defined by the dink engine and can't be changed

//use this by by pass MFC key codes.
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)


#define SCREEN_WIDTH		600		// width in pixels, not memory size
#define SCREEN_HEIGHT		400

#define MAX_SEQUENCES		1000
#define MAX_FRAMES			50
#define MAX_FRAME_INFOS		500
#define NUM_HARD_TILES		800

#define NUM_SCREENS			768
#define MAP_COLUMNS			32
#define MAP_ROWS			24

#define SCREEN_TILE_WIDTH	12
#define SCREEN_TILE_HEIGHT	8

#define MAX_TILE_BMPS		41

#define TILEWIDTH			50
#define TILEHEIGHT			50

#define SIDEBAR_WIDTH		20

#define MAX_SPRITES			100	// only 99 used, 100 available in map.dat though

#define SCREEN_DATA_SIZE	31280

#define BASE_SCRIPT_MAX_LENGTH	21

// not defined by the dink engine

#define DEFAULT_MAX_UNDO_LEVEL	50
#define DEFAULT_SCREEN_GAP	    3
#define DEFAULT_AUTOSAVE_TIME   0

#define VISIONS_TRACKED				256

#define MOUSE_BOX_LINE_WIDTH		1
#define SPRITE_SELECTOR_BMP_GAP		4
#define TILE_SELECTOR_GAP			4

#define SHOW_SCREEN						0
#define SHOW_MINIMAP					1
#define SHOW_SPRITE_SELECTOR			2
#define SHOW_TILE_SELECTOR				3
#define SHOW_HARD_TILE_SELECTOR			4
#define SHOW_SCREEN_IMPORTER			5
#define SHOW_TILE_HARDNESS_EDITOR		6
#define SHOW_SPRITE_HARDNESS_EDITOR		7

#define SPRITE_MODE		0
#define TILE_MODE		1
#define HARDBOX_MODE	2

#define MAX_PARSE_INPUTS	10
#define MAX_LINE_LENGTH		500

#define BLACK		1
#define NOTANIM		2
#define LEFTALIGN	3

#define SPRITE_SELECTOR_BMP_WIDTH	40
#define SPRITE_SELECTOR_BMP_HEIGHT	40

#define SQUARE_WIDTH		20
#define SQUARE_HEIGHT		20

//primary colors
#define COLOR_BLACK		RGB(0,0,0)
#define COLOR_RED		RGB(255,0,0)
#define COLOR_YELLOW	RGB(255,255,0)
#define COLOR_GREEN		RGB(0,255,0)
#define COLOR_BLUE		RGB(0,0,255)
#define COLOR_WHITE		RGB(255,255,255)

//timers
#define TIMER_AUTO_SAVE 1
#define TIMER_MOUSE_CHECK 2

// STRUCTS ////////////////////////////////////////////////

class Map;
class Tile;
class SpriteEditor;

// GLOBALS ////////////////////////////////////////////////

extern HWND main_window_handle; // save the window handle

extern LPDIRECTDRAW7			lpdd7;			// dd4 object
extern LPDIRECTDRAWSURFACE7		lpddsprimary;	// dd primary surface
extern LPDIRECTDRAWSURFACE7		lpddsback;		// dd back surface
extern LPDIRECTDRAWSURFACE7		lpddshidden;    // dd hidden surface
extern LPDIRECTDRAWCLIPPER		lpddclipper;	// dd clipper
extern LPDIRECTDRAWCLIPPER		lpddclipperprimary;	// dd clipper
extern DDSURFACEDESC2			ddsdback;		// a direct draw surface description struct
extern DDSURFACEDESC2			ddsdhidden;

extern DDBLTFX hardFill;	// used for sequences

extern RECT mapRect;	// location of the map on the screen

extern Map* current_map;

extern Tile* tileBmp[MAX_TILE_BMPS];

extern SpriteEditor* spriteeditor;

extern GraphicsManager graphics_manager;

extern UCHAR vision_used[VISIONS_TRACKED];
extern UCHAR new_vision_used[VISIONS_TRACKED];

extern bool need_resizing;
extern bool sprite_hard;
extern bool displayhardness;
extern bool show_sprite_info;
extern bool show_minimap_progress;
extern bool optimize_dink_ini;
extern bool fast_minimap_update;
extern bool hover_sprite_info;
extern bool hover_sprite_hardness;
extern bool help_text;

extern int screen_gap;
extern int max_undo_level;
extern int auto_save_time;
extern int tile_brush_size;
extern int help_text_color;

extern bool update_minimap;

extern CString main_dink_path;	// pathname to dink directory
extern CString dink_path;	// pathname to dink dmod directory
extern CString Dmod_Name;	// used for bypassing crappy mfc document structures
extern CString WDE_path;	// path to the windinkedit directory

#endif