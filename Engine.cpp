// INCLUDES ///////////////////////////////////////////////

#include "StdAfx.h"

#include "WinDinkeditView.h"
#include "MainFrm.h"

#include <ddraw.h>  // include directdraw

#include "Globals.h"
#include "Engine.h"
#include "Sequence.h"
#include "Minimap.h"
#include "Screen.h"
#include "ddutil.h"
#include "Map.h"
#include "SpriteSelector.h"
#include "SpriteEditor.h"
#include "TileSelector.h"
#include "Colors.h"
#include "ImportMap.h"
#include "Interface.h"
#include "Structs.h"
#include "Common.h"

// DEFINES ////////////////////////////////////////////////

// MACROS /////////////////////////////////////////////////

//#define ABS	(var) ((var < 0) ? (var) : (-var))

// TYPES //////////////////////////////////////////////////

// PROTOTYPES /////////////////////////////////////////////

// GLOBALS ////////////////////////////////////////////////

DDBLTFX			screen_grid_fill;	// used to fill backbuffer

bool draw_box = false;

int mouse_x_position, mouse_y_position;
int mouse_x_origin, mouse_y_origin;

// FUNCTIONS //////////////////////////////////////////////


int Game_Init()
{
	// read the WDE ini file
	readWDEIni();
	
	if (FAILED(DirectDrawCreateEx(NULL, (VOID**)&lpdd7, IID_IDirectDraw7, NULL)))
		return FALSE;

	// set cooperation to normal since this will be a windowed app
	if (FAILED(lpdd7->SetCooperativeLevel(AfxGetMainWnd()->GetSafeHwnd(), DDSCL_NORMAL)))
		return FALSE;

	// clear ddsd and set size
	DDSURFACEDESC2 ddsd;
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	// create the primary surface
	if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsprimary, NULL)))
		return FALSE;

	setColors();

	// fill in ddbltfx structure which is used to turn screen black before drawing
	memset(&screen_grid_fill,0,sizeof(DDBLTFX));
	screen_grid_fill.dwSize = sizeof(DDBLTFX);


	memset(&hardFill,0,sizeof(DDBLTFX));
	hardFill.dwSize = sizeof(DDBLTFX);

	// Attach a clipper to the primary surface
    if (lpdd7->CreateClipper(0, &lpddclipperprimary, NULL) != DD_OK)
        return FALSE;

    if (lpddclipperprimary->SetHWnd(0, AfxGetMainWnd()->GetSafeHwnd()) != DD_OK)
        return FALSE;

    if (lpddsprimary->SetClipper(lpddclipperprimary) != DD_OK)
        return FALSE;

	
	memset(&ddsdhidden,0,sizeof(ddsdhidden));	// clear out the structure first
	ddsdhidden.dwSize  = sizeof(ddsdhidden);
	ddsdhidden.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// set dimensions of the new bitmap surface
	ddsdhidden.dwWidth  = 640;		// width in pixels, not memory size
	ddsdhidden.dwHeight = 480;
	ddsdhidden.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;

	HRESULT hr;

	if (FAILED(hr = lpdd7->CreateSurface(&ddsdhidden, &lpddshidden, NULL)))
	{
		if (hr == DDERR_OUTOFVIDEOMEMORY)
		{
			if (graphics_manager.videoCardOnlyMemory)
			{
				while (TRUE)
				{
					// delete graphics in increments of 500k until there is enough
					// video memory for the backbuffer
					if (graphics_manager.removeGraphics(500000) == TRUE)
					{
						if ((hr = lpdd7->CreateSurface(&ddsdhidden, &lpddshidden, NULL)) == DD_OK)
						{
							break;
						}
					}
					else
					{
						graphics_manager.videoCardOnlyMemory = FALSE;
						ddsdhidden.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
						break;
					}
				}
			}
			else
			{
				// first try unloading all graphics that are in video memory except
				// for the minimap
				graphics_manager.removeAllVideoMemGraphics();
				if (FAILED(hr = lpdd7->CreateSurface(&ddsdhidden, &lpddshidden, NULL)))
				{
					// still need more video memory, unload the minimap
					delete current_map->minimap;
					if (FAILED(hr = lpdd7->CreateSurface(&ddsdhidden, &lpddshidden, NULL)))
					{
						
					}
					// reload the minimap, loses all information though
					current_map->minimap = new Minimap();
				}
			}
		}
	}

	if (lpddshidden == NULL)
	{
		ddsdhidden.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

		if (FAILED(hr = lpdd7->CreateSurface(&ddsdhidden, &lpddshidden, NULL)))
			return false;
	}

	spriteeditor = new SpriteEditor();

	screen_grid_fill.dwFillColor = color_map_screen_grid;

	graphics_manager.getVideoMemory();

	initializeInput();

	if (auto_save_time)
		SetTimer(mainWnd->GetSafeHwnd(), TIMER_AUTO_SAVE, auto_save_time * 60000, NULL);

	return TRUE;
}


int RestoreSurfaces()
{
	memset(current_map->miniupdated,0,sizeof(current_map->miniupdated));

	for (int i = 0; i < MAX_SEQUENCES; i++)
	{
		for (int y = 0; y < MAX_FRAMES; y++)
		{
			if (current_map->sequence[i])
			{
				if (current_map->sequence[i]->frame_image[y])
				{
					current_map->sequence[i]->frame_image[y]->Release();
					current_map->sequence[i]->loadFrame(y);
				}
			}
		}
	}


  if(lpddsprimary)
		if(lpddsprimary->IsLost())
			lpddsprimary->Restore();

	if(lpddsback)
		if(lpddsback->IsLost())
			lpddsback->Restore();

	if (lpddshidden)
		if (lpddshidden->IsLost())
			lpddshidden->Restore();

	current_map->minimap->restoreSurfaces();

	return TRUE;
}

// resizes backbuffer whenever window size is changed
int resize_map_window(int new_width, int new_height)
{
	need_resizing = false;

	// first delete the secondary surface and clipper
	if (lpddsback != NULL)
	{
		lpddsback->Release();
		lpddsback = NULL;
	}
	if (lpddclipper != NULL)
	{
		lpddclipper->Release();
		lpddclipper = NULL;
	}

	// now create the secondary surface and clipper again

	// Now we have to create an alternative surface for the flipping chain
	memset(&ddsdback,0,sizeof(ddsdback));	// clear out the structure first
	ddsdback.dwSize  = sizeof(ddsdback);
	ddsdback.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// set dimensions of the new bitmap surface
	ddsdback.dwWidth  = new_width;		// width in pixels, not memory size
	ddsdback.dwHeight = new_height;
	ddsdback.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;

	HRESULT hr;

	if (FAILED(hr = lpdd7->CreateSurface(&ddsdback, &lpddsback, NULL)))
	{
		if (hr == DDERR_OUTOFVIDEOMEMORY)
		{
			if (graphics_manager.videoCardOnlyMemory)
			{
				while (TRUE)
				{
					// delete graphics in increments of 500k until there is enough
					// video memory for the backbuffer
					if (graphics_manager.removeGraphics(500000) == TRUE)
					{
						if ((hr = lpdd7->CreateSurface(&ddsdback, &lpddsback, NULL)) == DD_OK)
						{
							break;
						}
					}
					else
					{
						graphics_manager.videoCardOnlyMemory = FALSE;DDSCAPS_SYSTEMMEMORY;
						break;
					}
				}
			}
			else
			{
				// first try unloading all graphics that are in video memory except
				// for the minimap
				graphics_manager.removeAllVideoMemGraphics();
				if (FAILED(hr = lpdd7->CreateSurface(&ddsdback, &lpddsback, NULL)))
				{
					// still need more video memory, unload the minimap
					delete current_map->minimap;
					if (FAILED(hr = lpdd7->CreateSurface(&ddsdback, &lpddsback, NULL)))
					{
						
					}
					// reload the minimap, loses all information though
					current_map->minimap = new Minimap();
				}
			}
		}
	}

	//
	if (lpddsback == NULL)
	{
		// set surface to offscreen plain
		ddsdback.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

		// create the surface
		if (FAILED(hr = lpdd7->CreateSurface(&ddsdback, &lpddsback, NULL)))
			return FALSE;
	}

	// set clipping rectangle to screen extents so objects dont
	// mess up at edges
	RECT screen_rect = {0, 0, new_width, new_height};
//	lpddclipper = DD_Attach_Clipper(lpddsback, screen_rect);

	// reset the map position, only right and bottom side change
	current_map->window_width = new_width;
	current_map->window_height = new_height;
	current_map->updateMapPosition(current_map->window.left, current_map->window.top);

	current_map->sprite_selector.resizeScreen();
	current_map->hard_tile_selector.resizeScreen();
	current_map->minimap->resizeScreen();

	return TRUE;
}
///////////////////////////////////////////////////////////

void Game_Shutdown()
{
	graphics_manager.empty();

	// release the clipper
	if (lpddclipper != NULL)
	{
		lpddclipper->Release();
		lpddclipper = NULL;
	}

	// release the secondary surface
	if (lpddsback != NULL)
	{
		lpddsback->Release();
		lpddsback = NULL;
	}

	// release the primary surface clipper
	if (lpddclipperprimary != NULL)
	{
		lpddclipperprimary->Release();
		lpddclipperprimary = NULL;
	}

	// release the primary surface
	if (lpddsprimary != NULL)
	{
		lpddsprimary->Release();
		lpddsprimary = NULL;
	}

	// release the hidden surface
	if (lpddshidden != NULL)
	{
		lpddshidden->Release();
		lpddshidden = NULL;
	}

	// release the directdraw object
	if (lpdd7 != NULL)
	{
		lpdd7->Release();
		lpdd7 = NULL;
	}

	// write the WDE ini file
	writeWDEIni();
}

///////////////////////////////////////////////////////////

int Game_Main(void)
{
	// if a dmod isn't open or the screen doesn't have focus don't do anything
	if (!current_map)
		return false;

	// check if back buffer needs to be resized
	if (need_resizing)
	{
		int new_width = mapRect.right - mapRect.left;
		int new_height = mapRect.bottom - mapRect.top;
	
		//if the window is too small to resize the back buffer
		//return out of the main so nothing draws. Otherwise
		//the program will crash.
		if (new_width <= 1 || new_height <= 1)
			return false;

		// make sure it needs resizing first
		if (new_width != current_map->window_width || new_height != current_map->window_height)
		{
			// now resize the map
			resize_map_window(new_width, new_height);
		}
	}

	// draw the main screen
	if (current_map->render_state == SHOW_SCREEN)
	{
		// fills the back buffer with black
		lpddsback->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &screen_grid_fill);
		
		// draw the screen(s)
		current_map->drawScreens();

		if (current_map->screenMode == TILE_MODE)
		{
			if (draw_box)
			{
				RECT dest = {mouse_x_origin, mouse_y_origin, mouse_x_position, mouse_y_position};
				(dest, color_mouse_box, 1);
			}
			else
			{
				current_map->tile_selector.renderScreenTileHover(mouse_x_position, mouse_y_position);
			}
			current_map->tile_selector.screenRender();

			ScreenText("Press F12 to Toggle this text, and F5 to change it's color.", 90);
			ScreenText("Press 'R' for Sprite mode, 'T' for Tile Mode, and 'H' for Hardness Mode", 70);
			ScreenText("Click to Select a tile (click and drag for multiple tiles).", 50);
			ScreenText("Press 'C' to copy tiles, and press 'S' to stamp the tiles", 30);
			
			
		}
		else if (current_map->screenMode == HARDBOX_MODE)
		{
			if (draw_box)
			{
				RECT dest = {mouse_x_origin, mouse_y_origin, mouse_x_position, mouse_y_position};
				(dest, color_mouse_box, 1);
			} else 
			{
				current_map->hard_tile_selector.renderScreenTileHover(mouse_x_position, mouse_y_position);
			}
			current_map->hard_tile_selector.screenRender();
		
			ScreenText("Press F12 to Toggle this text, and F5 to change it's color.", 90);
			ScreenText("Press 'R' for Sprite mode, 'T' for Tile Mode, and 'H' for Hardness Mode.", 70);
			ScreenText("Click to Select a tile. Press 'C' to copy tiles,",50);
			ScreenText("press 'Q' to edit the copied hardness, and press 'S' to stamp the tiles.", 30);
			
		} else {
			ScreenText("Press F12 to Toggle this text, and F5 to change it's color.", 110);
			ScreenText("Press 'R' for Sprite mode, 'T' for Tile Mode, and 'H' for Hardness Mode", 90);
			ScreenText("Click a sprite to pick it up. Right click to view it's properties.", 70);
			ScreenText("Right click a map square to view it's details.", 50);
			ScreenText("Press 'M' for screen match, space bar for hardness view, and 'I' for info", 30);
		}
	}
	// check if minimap needs to be shown
	else if (current_map->render_state == SHOW_MINIMAP)
	{
		// draw a box around the selected tile
		current_map->minimap->render();

		ScreenText("Press F12 to Toggle this text, and F5 to change it's color.", 70);
		ScreenText("Press Spacebar to detail minimap. Click a square to view it.", 50);
		ScreenText("Right click on any map square to select it's details.", 30);
	}
	// check if sprite selector should be shown
	else if (current_map->render_state == SHOW_SPRITE_SELECTOR)
	{
		// fills the back buffer with black
		lpddsback->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &screen_grid_fill);
		current_map->sprite_selector.render();
		current_map->sprite_selector.drawGrid(mouse_x_position, mouse_y_position);
	}
	// check if tile selector should be shown
	else if (current_map->render_state == SHOW_TILE_SELECTOR)
	{
		// fills the back buffer with black
		lpddsback->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &screen_grid_fill);
		current_map->tile_selector.render();

		if (draw_box)
		{
			RECT dest = {mouse_x_origin, mouse_y_origin, mouse_x_position, mouse_y_position};
			drawBox(dest, color_mouse_box, 1);
		}
		else
		{
			current_map->tile_selector.renderTileHover(mouse_x_position, mouse_y_position);
		}
	}
	// check if sprite selector should be shown
	else if (current_map->render_state == SHOW_HARD_TILE_SELECTOR)
	{
		// fills the back buffer with black
		lpddsback->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &screen_grid_fill);
		current_map->hard_tile_selector.render();
		current_map->hard_tile_selector.drawGrid(mouse_x_position, mouse_y_position);
	}
	else if (current_map->render_state == SHOW_SCREEN_IMPORTER)
	{
		current_map->import_map->render();
	}
	else if (current_map->render_state == SHOW_TILE_HARDNESS_EDITOR)
	{
		// fills the back buffer with black
		lpddsback->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &screen_grid_fill);
		current_map->hard_tile_selector.displayEditor(mouse_x_position, mouse_y_position);

		ScreenText("Press F12 to Toggle this text, and F5 to change it's color.", 70);
		ScreenText("Click a hard box to change the hardness.", 50);
		ScreenText("Hold Control for low hardness, and Shift for unknown hardness.", 30);
	}
	else if (current_map->render_state == SHOW_SPRITE_HARDNESS_EDITOR)
	{
		if (!draw_box)
		{
			// fills the back buffer with black
			lpddsback->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &screen_grid_fill);
			spriteeditor->DrawSprite();
			spriteeditor->DrawHardness();
		}

		ScreenText("Press F12 to Toggle this text, and F5 to change it's color.", 70);
		ScreenText("Click and drag to change the hard box. Right click to change the Depth Que", 50);
		ScreenText("Press Escape to exit.", 30);

	}

	HRESULT hr = lpddsprimary->Blt(&mapRect, lpddsback, NULL, DDBLT_WAIT, 0);

	if(hr == DDERR_SURFACELOST)
		RestoreSurfaces();

	graphics_manager.checkMemory();

	return true;
}

///////////////////////////////////////////////////////

LPDIRECTDRAWCLIPPER DD_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds, RECT clip_list)
{
	// this function creates a clipper from the sent clip list and attaches
	// it to the sent surface

	LPDIRECTDRAWCLIPPER lpddClipper;   // pointer to the newly created dd clipper
	LPRGNDATA region_data;             // pointer to the region data that contains
	// the header and clip list

	// first create the direct draw clipper
	if ((lpdd7->CreateClipper(0, &lpddClipper, NULL)) != DD_OK)
		return NULL;

	// now create the clip list from the sent data
	region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER) + sizeof(RECT));

	// copy the rects into region data
	memcpy(region_data->Buffer, &clip_list, sizeof(RECT));

	// set up fields of header
	region_data->rdh.dwSize          = sizeof(RGNDATAHEADER);
	region_data->rdh.iType           = RDH_RECTANGLES;
	region_data->rdh.nCount          = 1;
	region_data->rdh.nRgnSize        = sizeof(RECT);

	region_data->rdh.rcBound.left    = clip_list.left;
	region_data->rdh.rcBound.top     = clip_list.top;
	region_data->rdh.rcBound.right   = clip_list.right;
	region_data->rdh.rcBound.bottom  = clip_list.bottom;
	
	if ((lpddClipper->SetClipList(region_data, 0)) != DD_OK)
	{
		free(region_data);
		return NULL;
	}

	// now attach the clipper to the surface
	if ((lpdds->SetClipper(lpddClipper)) != DD_OK)
	{
		free(region_data);
		return NULL;
	}

	// all is well, so release memory and send back the pointer to the new clipper
	free(region_data);

	return lpddClipper;
}