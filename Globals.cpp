#include "StdAfx.h"

//#include "WinDinkeditView.h"

#include <ddraw.h>

#include "Globals.h"
#include "Map.h"
#include "Engine.h"
#include "ddutil.h"
#include "Colors.h"

// GLOBALS ////////////////////////////////////////////////

LPDIRECTDRAW7			lpdd7			= NULL;		// dd7 object
LPDIRECTDRAWSURFACE7	lpddsprimary	= NULL;		// dd primary surface
LPDIRECTDRAWSURFACE7	lpddsback		= NULL;		// dd back surface
LPDIRECTDRAWSURFACE7	lpddshidden		= NULL;     // dd hidden surface
LPDIRECTDRAWCLIPPER		lpddclipper		= NULL;		// dd clipper
LPDIRECTDRAWCLIPPER		lpddclipperprimary = NULL;	// dd clipper
DDSURFACEDESC2			ddsdback;		// a direct draw surface description struct
DDSURFACEDESC2			ddsdhidden;		// a direct draw surface description struct

DDBLTFX hardFill;

RECT mapRect;	// stores the location of the drawing window

Map* current_map = NULL;

Tile* tileBmp[MAX_TILE_BMPS];

SpriteEditor* spriteeditor;

GraphicsManager graphics_manager;

UCHAR vision_used[VISIONS_TRACKED];
UCHAR new_vision_used[VISIONS_TRACKED];

bool need_resizing = true;
bool displayhardness = false;
bool sprite_hard = false;
bool show_sprite_info = false;
bool show_minimap_progress = true;
bool optimize_dink_ini = false;
bool fast_minimap_update = true;
bool hover_sprite_info = false;
bool hover_sprite_hardness = false;
bool help_text = true;
bool update_minimap = false;

int help_text_color = RGB(255,255,0);

int screen_gap = 1;
int max_undo_level = 50;
int auto_save_time = 0;
int tile_brush_size = 0;

CString main_dink_path;	// pathname to dink directory
CString dink_path;	// pathname to dink dmod directory
CString Dmod_Name;
CString WDE_path;
