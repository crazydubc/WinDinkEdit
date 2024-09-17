#include "StdAfx.h"

#include "HardTileSelector.h"
#include "Map.h"
#include "Globals.h"
#include "ddutil.h"
#include "Engine.h"
#include "Screen.h"
#include "Tile.h"
#include "MainFrm.h"
#include "Colors.h"
#include "GraphicsManager.h"
#include "Common.h"
#include "Undo.h"

HardTileSelector::HardTileSelector()
{
	selectorScreen = 0;
	tile_box_points = 0;

	screen_first_x_tile = -1;
	y_tiles_selected = 0;
	x_tiles_selected = 0;

	memset(hardTileSurface, 0, NUM_HARD_TILES * 4);
	memset(graphic_que_location, 0, NUM_HARD_TILES * 4);
	memset(hard_tile_set, 0, sizeof(hard_tile_set));
}

HardTileSelector::~HardTileSelector()
{

}

//called from MFC class
void HardTileSelector::SetDefaultHardTile()
{
	if (x_tiles_selected < 1 || y_tiles_selected < 1)
	{
		MessageBox(mainWnd->GetSafeHwnd(), "Please copy a tile onto the clip board before attempting to set a default hard tile.", "Error: Clipboard empty", NULL);
		return;
	}

	//get screen
	int cur_screen = MAP_COLUMNS * (screen_first_y_tile / SCREEN_TILE_HEIGHT) + screen_first_x_tile / SCREEN_TILE_WIDTH;

	//get current tile
	int x_tile = screen_first_x_tile % SCREEN_TILE_WIDTH;
	int y_tile = screen_first_y_tile % SCREEN_TILE_HEIGHT;

	//get tile data
	TILEDATA tile = current_map->screen[cur_screen]->tiles[y_tile][x_tile];


	//fill in the undo
	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_SET_DEFAULT_HARD_TILE;
	action->set_default_hard_tile.x = tile.x;
	action->set_default_hard_tile.y = tile.y;
	action->set_default_hard_tile.bmp = tile.bmp;
	action->set_default_hard_tile.old_index = tileBmp[tile.bmp]->tile_hardness[tile.y][tile.x];
	action->set_default_hard_tile.new_index = hard_tile_set[0][0];

	current_map->undo_buffer->addUndo(action);
}

//called from MFC class
void HardTileSelector::TransformHardTile(int hardness)
{
	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_TRANSFORM_HARD_TILE;

	//get screen
	int cur_screen = MAP_COLUMNS * (screen_first_y_tile / SCREEN_TILE_HEIGHT) + screen_first_x_tile / SCREEN_TILE_WIDTH;

	//get current tile
	int x_tile = screen_first_x_tile % SCREEN_TILE_WIDTH;
	int y_tile = screen_first_y_tile % SCREEN_TILE_HEIGHT;

	int tile_num;

	if (current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness)
	{
		tile_num = current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness;
	} else {
		TILEDATA tile = current_map->screen[cur_screen]->tiles[y_tile][x_tile];
		tile_num = tileBmp[tile.bmp]->tile_hardness[tile.y][tile.x];
	}

	action->transform_hard_tile.tile_num = tile_num;
	action->transform_hard_tile.hardness = hardness;

	memcpy(action->transform_hard_tile.old_tile_data, hardTile[tile_num], 2500);

	current_map->undo_buffer->addUndo(action);
}

// deletes the current hardness on a tile and reverts it back to it's natural hard tile
int HardTileSelector::revertTile()
{
	//is there any tiles selected?
	if (screen_first_x_tile == -1)
	{
		MessageBox(NULL, "You must click on one or more tile destinations before tile(s) can be placed.", "Cannot Place Tile(s)", MB_OK);
		return false;
	}

	//lets get the demensions of what the usee wants to paste
	//make sure we don't go out of bounds
	int width = screen_last_x_tile - screen_first_x_tile + 1;

	int height = screen_last_y_tile - screen_first_y_tile + 1;

	//add to undo buffer
	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_HARD_TILE_CHANGE;
	action->hard_tile_change.first_tile_x = screen_first_x_tile;
	action->hard_tile_change.first_tile_y = screen_first_y_tile;
	action->hard_tile_change.width = width;
	action->hard_tile_change.height = height;

	int cur_screen, x_tile, y_tile, x, y;

	memset(&action->hard_tile_change.new_hard_tiles, 0, sizeof(action->hard_tile_change.new_hard_tiles));
	memset(&action->hard_tile_change.old_hard_tiles, 0, sizeof(action->hard_tile_change.old_hard_tiles));

	//save old tiles
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			//check screen we could have moved onto a new screen, or we could still be on it.
			cur_screen = MAP_COLUMNS * ((y + screen_first_y_tile) / SCREEN_TILE_HEIGHT) + (x + screen_first_x_tile) / SCREEN_TILE_WIDTH;

			if (cur_screen > 768 || cur_screen < 1)
				continue;

			//make sure the screen is valid
			if (current_map->screen[cur_screen] == NULL)
				continue;
			
			//zoom into the tile in question
			x_tile = (x + screen_first_x_tile) % SCREEN_TILE_WIDTH;
			y_tile = (y + screen_first_y_tile) % SCREEN_TILE_HEIGHT;

			//copy it over if it is there
			if (current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness)
			{
				action->hard_tile_change.old_hard_tiles[x][y] = current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness;
			}
		}
	}

	current_map->undo_buffer->addUndo(action);

	return true;
}

int HardTileSelector::renderScreenTileHover(int x, int y)
{
	int x_screen = (x + current_map->window.left) / (SCREEN_WIDTH + screen_gap);
	int y_screen = (y + current_map->window.top) / (SCREEN_HEIGHT + screen_gap);

	int cur_screen = y_screen * MAP_COLUMNS + x_screen;

	if (current_map->screen[cur_screen] == NULL)
		return false;

	int x_tile = ((x + current_map->window.left) % (SCREEN_WIDTH + screen_gap)) / TILEWIDTH;
	int y_tile = ((y + current_map->window.top) % (SCREEN_HEIGHT + screen_gap)) / TILEHEIGHT;

	int x_offset = x_screen * (SCREEN_WIDTH + screen_gap) + x_tile * TILEWIDTH - current_map->window.left;
	int y_offset = y_screen * (SCREEN_HEIGHT + screen_gap) + y_tile * TILEHEIGHT - current_map->window.top;

	RECT dest = {x_offset, y_offset, x_offset + TILEWIDTH, y_offset + TILEHEIGHT};
	drawBox(dest, color_tile_grid, 1);

	// possibly draw box
	for (int i = 1; i < tile_box_points; i++)
	{
		drawLine(tile_box_x[i - 1], tile_box_y[i - 1], tile_box_x[i], tile_box_y[i], color_polygon_fill_hardness);
	}

	if (tile_box_points > 0 && tile_box_points < MAX_TILE_BOX_POINTS)
	{
		drawLine(mouse_x_position, mouse_y_position, tile_box_x[0], tile_box_y[0], color_polygon_fill_hardness);
	}

	if (tile_box_points == MAX_TILE_BOX_POINTS)
	{
		drawLine(tile_box_x[0], tile_box_y[0], 
			tile_box_x[MAX_TILE_BOX_POINTS - 1], tile_box_y[MAX_TILE_BOX_POINTS - 1], color_polygon_fill_hardness);

		placeTileBox();
	}

	if (tile_box_points < MAX_TILE_BOX_POINTS && tile_box_points > 1)
	{
		drawLine(mouse_x_position, mouse_y_position, 
			tile_box_x[tile_box_points - 1], tile_box_y[tile_box_points - 1], color_polygon_fill_hardness);
	}

	return true;
}

int HardTileSelector::nextPage()
{
	if ((selectorScreen + 1) * max_pics < NUM_HARD_TILES)
		selectorScreen++;

	return true;
}

int HardTileSelector::prevPage()
{
	if (selectorScreen > 0)
		selectorScreen--;

	return true;
}

int HardTileSelector::resizeScreen()
{
	tiles_per_row = current_map->window_width / (TILEWIDTH + HARD_TILE_SELECTOR_BMP_GAP);
	tiles_per_column = current_map->window_height / (TILEHEIGHT + HARD_TILE_SELECTOR_BMP_GAP);

	max_pics = tiles_per_row * tiles_per_column;

	selectorScreen = 0;

	return true;
}

// draw the 
int HardTileSelector::render()
{
	//+1 so it wont display 0. 0 is bad...
	pics_displayed = (selectorScreen * max_pics) + 1;
	
	for (int y = 0; (y < tiles_per_column) & (pics_displayed < NUM_HARD_TILES); y++)
	{
		for (int x = 0; (x < tiles_per_row) & (pics_displayed < NUM_HARD_TILES); x++)
		{
			renderHardness(x * (TILEWIDTH + HARD_TILE_SELECTOR_BMP_GAP), 
				y * (TILEHEIGHT + HARD_TILE_SELECTOR_BMP_GAP), pics_displayed, false);
			pics_displayed++;
		}
	}

	return true;
}

int HardTileSelector::renderHardness(int hard_tile_num, bool transparent)
{
	if (hard_tile_num <= 0)
		return false;

	RECT src_rect = {0, 0, TILEWIDTH, TILEHEIGHT};
	RECT dest_rect = {0, 0, box_width * 50, box_height * 50};

	if (current_map->hard_tile_selector.hardTileSurface[hard_tile_num] == NULL)
		current_map->hard_tile_selector.loadSurface(hard_tile_num);

  	if (current_map->hard_tile_selector.hardTileSurface[hard_tile_num])
	{
		graphics_manager.moveLocation(current_map->hard_tile_selector.graphic_que_location[hard_tile_num]);

		if (transparent)
		{
			// blt to destination surface
			lpddsback->Blt(&dest_rect, current_map->hard_tile_selector.hardTileSurface[hard_tile_num], &src_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);
		}
		else
		{
			lpddsback->Blt(&dest_rect, current_map->hard_tile_selector.hardTileSurface[hard_tile_num], &src_rect, DDBLT_WAIT, NULL);
		}
	}
	
	return true;
}

// 
int HardTileSelector::drawGrid(int x, int y)
{
	int x_tile = x / (TILEWIDTH + HARD_TILE_SELECTOR_BMP_GAP);
	int y_tile = y / (TILEHEIGHT + HARD_TILE_SELECTOR_BMP_GAP);

	int pic_selected = y_tile * tiles_per_row + x_tile + selectorScreen * max_pics;

	if (pic_selected >= NUM_HARD_TILES)
		return false;

	RECT dest = {(TILEWIDTH + HARD_TILE_SELECTOR_BMP_GAP) * x_tile - HARD_TILE_SELECTOR_BMP_GAP,
			(TILEHEIGHT + HARD_TILE_SELECTOR_BMP_GAP) * y_tile - HARD_TILE_SELECTOR_BMP_GAP,
			(TILEWIDTH + HARD_TILE_SELECTOR_BMP_GAP) * x_tile + TILEWIDTH + HARD_TILE_SELECTOR_BMP_GAP,
			(TILEHEIGHT + HARD_TILE_SELECTOR_BMP_GAP) * y_tile + TILEHEIGHT + HARD_TILE_SELECTOR_BMP_GAP};
	drawBox(dest, color_tile_grid, HARD_TILE_SELECTOR_BMP_GAP);

	char buffer[50];
	wsprintf(buffer, "Current Hard Tile: %d", pic_selected);
	mainWnd->setStatusText(buffer);

	return true;
}


int HardTileSelector::LoadEditor()
{
	current_map->render_state = SHOW_TILE_HARDNESS_EDITOR;
	if (screen_first_x_tile < 1 || screen_first_y_tile < 1)
	{
		MessageBox(mainWnd->GetSafeHwnd(), "Please Select a tile before trying to edit it.", NULL, NULL);
		current_map->render_state = SHOW_SCREEN;
		return false;
	}

	//get screen
	int cur_screen = MAP_COLUMNS * (screen_first_y_tile / SCREEN_TILE_HEIGHT) + screen_first_x_tile / SCREEN_TILE_WIDTH;

	//get current tile
	int x_tile = screen_first_x_tile % SCREEN_TILE_WIDTH;
	int y_tile = screen_first_y_tile % SCREEN_TILE_HEIGHT;

	//get tile data
	if (current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness == 0)
	{
		TILEDATA temp = current_map->screen[cur_screen]->tiles[y_tile][x_tile];
		cur_hard_num = tileBmp[temp.bmp]->tile_hardness[temp.y][temp.x];
	}
	else
	{
		cur_hard_num = current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness;
	}

	memcpy(current_hardness, hardTile[cur_hard_num], sizeof(current_hardness));
	memcpy(&cur_tile, &current_map->screen[cur_screen]->tiles[y_tile][x_tile], sizeof(TILEDATA));

	DrawEditor();

	return true;
}

int HardTileSelector::DrawEditor()
{
	int box_width = 450 / 50;
	int box_height = 450 / 50;

	RECT dest_rect = {0,0,450,450};

	if (cur_tile.bmp >= 0 && cur_tile.bmp < 41)
	{
		tileBmp[cur_tile.bmp]->render(dest_rect, cur_tile.x, cur_tile.y, lpddshidden);
	}

	int color;

	for (int x = 0; x < 50; x++)
	{
		for (int y = 0; y < 50; y++)
		{
			switch (current_hardness[y][x])
			{
			case 0:
				{
					color = NULL;
				} break;

			case 1:
				{
					color = color_normal_tile_hardness;
				} break;

			case 2:
				{
					color = color_low_tile_hardness;
				} break;

			case 3:
				{
					color = color_other_tile_hardness;
				} break;
			default:
				{
					color = NULL;
				} break;
			}

			if (color)
			{
				dest_rect.top = y * box_height;
				dest_rect.bottom = (y + 1) * box_height;
				dest_rect.left = x * box_width;
				dest_rect.right = (x + 1) * box_width;
				drawFilledBox(dest_rect, color, false, lpddshidden);
			}
		}
	}

	return true;
}

int HardTileSelector::displayEditor(int x, int y)
{
	int box_width = (current_map->window_width / 50);
	int box_height = (current_map->window_height / 50);

	int x_tile = x / box_width;
	int y_tile = y / box_height;

	if ((KEY_DOWN(VK_LBUTTON) || KEY_DOWN(VK_MBUTTON)) && draw_box)
	{
		for (x = 0; x < tile_brush_size; x++)
		{
			for (y = 0; y < tile_brush_size; y++)
			{
				if ((y_tile + y) >= 50 || (x_tile + x) >= 50)
					continue;

				if (drag_hardness == -1)
				{
					int size = int (current_hardness[y_tile+y][x_tile+x]);

					if (size)
					{
						if (size == 1 && KEY_DOWN(VK_CONTROL))
							drag_hardness = 2;
						else if (size == 1 && KEY_DOWN(VK_SHIFT))
							drag_hardness = 3;
						else if (size == 2 && KEY_DOWN(VK_SHIFT))
							drag_hardness = 3;
						else if (size == 3 && KEY_DOWN(VK_CONTROL))
							drag_hardness = 2;
						else
							drag_hardness = NULL;
					}
					else 
					{
						if (KEY_DOWN(VK_CONTROL))
							drag_hardness = 2;
						else if (KEY_DOWN(VK_SHIFT))
							drag_hardness = 3;
						else
							drag_hardness = 1;
					}
				}
				
				current_hardness[y_tile+y][x_tile+x] = UCHAR(drag_hardness);
			}
		}
		DrawEditor();
	} else {
		drag_hardness = -1;
	}

	
	RECT src_rect = {0,0,450,450};

	RECT dest_rect;

	dest_rect.top = 0;
	dest_rect.left = 0;
	dest_rect.bottom = box_height * 50;
	dest_rect.right = box_width * 50;

	lpddsback->Blt(&dest_rect, lpddshidden, &src_rect, DDBLT_WAIT, NULL);

	RECT dest = {box_width * x_tile,
		box_height * y_tile,
		(box_width * x_tile) + (box_width * tile_brush_size),
		(box_height * y_tile) + (box_height * tile_brush_size)};

	drawBox(dest, RGB(255,255,255), 1);

	return true;
}

int HardTileSelector::SaveEditor()
{
	current_map->render_state = SHOW_SCREEN;

	for (int x = 0; x < 50; x++)
	{
		for (int y = 0; y < 50; y++)
		{
			if (hardTile[cur_hard_num][x][y] != current_hardness[x][y])
			{
				UNDO_ACTION *action = new UNDO_ACTION;

				action->type = UT_HARD_TILE_EDIT;
				action->hard_tile_edit.hard_tile_num = cur_hard_num;
			
				memcpy(action->hard_tile_edit.old_tile_data, hardTile[cur_hard_num], sizeof(hardTile[cur_hard_num]));
				memcpy(action->hard_tile_edit.new_tile_data, current_hardness, sizeof(current_hardness));

				current_map->undo_buffer->addUndo(action);

				return true;
			}
		}
	}

	return true;
}

int HardTileSelector::getTile(int x, int y)
{
	tile.bmp = -1;
	// calculate the x and y location of the frame or sequence
	int x_tile = x / (TILEWIDTH + HARD_TILE_SELECTOR_BMP_GAP);
	int y_tile = y / (TILEHEIGHT + HARD_TILE_SELECTOR_BMP_GAP);

	// calculate the seqence number using number of sequences displayed
	int pic_selected = y_tile * tiles_per_row + x_tile + selectorScreen * max_pics;

	// make sure it is a valid sequence
	if (pic_selected >= NUM_HARD_TILES || pic_selected <= 0)
		return false;

	//copy into hard tile set.
	hard_tile_set[0][0] = pic_selected;
	x_tiles_selected = 1;
	y_tiles_selected = 1;

	mainWnd->setStatusText("");
	current_map->render_state = SHOW_SCREEN;
	Game_Main();

	return true;
}

// draws tile hardness
int HardTileSelector::renderHardness(int x, int y, int &hard_tile_num, bool transparent)
{
	if (hard_tile_num <= 0 || hard_tile_num > 800)
	{
		if (hard_tile_num == -1)
			return true;

		hard_tile_num = 0;
	}

	// fill in the destination rect
	RECT dest_rect;
	dest_rect.left   = x;
	dest_rect.top    = y;
	dest_rect.right  = x + TILEWIDTH;
	dest_rect.bottom = y + TILEHEIGHT;

	RECT src_rect = {0, 0, TILEWIDTH, TILEHEIGHT};
	RECT clip_rect = {0, 0, current_map->window_width, current_map->window_height};

	if (fixBounds(dest_rect, src_rect, clip_rect) == false)
		return false;

	if (hardTileSurface[hard_tile_num] == NULL)
		loadSurface(hard_tile_num);

  	if (hardTileSurface[hard_tile_num])
	{
		graphics_manager.moveLocation(graphic_que_location[hard_tile_num]);

		if (transparent)
		{
			// blt to destination surface
			lpddsback->Blt(&dest_rect, hardTileSurface[hard_tile_num], &src_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);
		}
		else
		{
			lpddsback->Blt(&dest_rect, hardTileSurface[hard_tile_num], &src_rect, DDBLT_WAIT, NULL);
		}
	}
	
	return true;
}

void HardTileSelector::unloadAllBitmaps()
{
	for (int i = 0; i < NUM_HARD_TILES; i++)
	{
		if (hardTileSurface[i])
		{
			hardTileSurface[i]->Release();
			hardTileSurface[i] = NULL;
			graphic_que_location[i] = NULL;
		}
	}
}

int HardTileSelector::unloadSurface(int hard_tile_num)
{
	if (hardTileSurface[hard_tile_num])
	{
		hardTileSurface[hard_tile_num]->Release();
		hardTileSurface[hard_tile_num] = NULL;
	}

	return true;
}

int HardTileSelector::loadSurface(int hard_tile_num)
{
	if (hardTileSurface[hard_tile_num])
		return true;

	DDSURFACEDESC2 ddsd; // create the surface description

	// set to access caps, width, and height
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// set dimensions of the new bitmap surface
	ddsd.dwWidth  = TILEWIDTH;		// width in pixels, not memory size
	ddsd.dwHeight = TILEHEIGHT;

	// create the surface
	hardTileSurface[hard_tile_num] = graphics_manager.allocateSurface(&ddsd);

	graphic_que_location[hard_tile_num] = graphics_manager.addGraphic(&hardTileSurface[hard_tile_num]);


	UCHAR *buffer = new UCHAR[TILEHEIGHT * TILEWIDTH * colorformat];

	int y;

	UCHAR *buf_ptr = hardTile[hard_tile_num][0];

	int color_index[4];
	color_index[0] = 0;
	color_index[1] = color_normal_tile_hardness;
	color_index[2] = color_low_tile_hardness;
	color_index[3] = color_other_tile_hardness;

	UCHAR *dest_ptr = buffer;

	if (colorformat == 2)
	{
		short *dest_ptr2 = (short*)buffer;
		short* src_ptr2 = (short*)color_index;

		for (int i = 0; i < TILEHEIGHT * TILEWIDTH; i++)
		{
			*dest_ptr2 = *(src_ptr2 + ((*buf_ptr) << 1));
			dest_ptr2++;

			buf_ptr++;
		}
	}
	else if (colorformat == 3)
	{
		UCHAR *dest_ptr2 = buffer;
		UCHAR* src_ptr2 = (UCHAR*)color_index;
		UCHAR* tmp_ptr;

		for (int i = 0; i < TILEHEIGHT * TILEWIDTH; i++)
		{
			tmp_ptr = src_ptr2 + (*buf_ptr << 2);
			*dest_ptr2 = *tmp_ptr;
			dest_ptr2++;
			tmp_ptr++;
			*dest_ptr2 = *tmp_ptr;
			dest_ptr2++;
			tmp_ptr++;
			*dest_ptr2 = *tmp_ptr;
			dest_ptr2++;
			tmp_ptr++;

			buf_ptr++;
		}
	}
	else if (colorformat == 4)
	{
		int *dest_ptr2 = (int*)buffer;

		for (int i = 0; i < TILEHEIGHT * TILEWIDTH; i++)
		{
			*dest_ptr2 = color_index[*buf_ptr];

			dest_ptr2++;
			buf_ptr++;
		}
	}

	UCHAR *source_ptr = buffer;

	// lock the display surface
	hardTileSurface[hard_tile_num]->Lock(NULL, &ddsd, (DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR), NULL);

	// find the destination bitmap pointer
	dest_ptr = (UCHAR *)ddsd.lpSurface;

	// iterate thru each scanline and copy bitmap
	for (y = 0; y < TILEHEIGHT; y++)
	{
		memcpy(dest_ptr, source_ptr, TILEWIDTH * colorformat);
		source_ptr += TILEWIDTH * colorformat;
		dest_ptr += ddsd.lPitch;
	}
	// unlock the surface 
	hardTileSurface[hard_tile_num]->Unlock(NULL);

	delete [] buffer;

	// set color key to color 0
	DDCOLORKEY color_key; // used to set color key
	color_key.dwColorSpaceLowValue  = 0;
	color_key.dwColorSpaceHighValue = 0;

	// now set the color key for source blitting
	hardTileSurface[hard_tile_num]->SetColorKey(DDCKEY_SRCBLT, &color_key);

	return true;
}
void HardTileSelector::placeTileBoxPoint(int x, int y)
{
	if (tile_box_points < MAX_TILE_BOX_POINTS)
	{
		tile_box_x[tile_box_points] = x;
		tile_box_y[tile_box_points] = y;
		tile_box_points++;

		// redraw the screen
		Game_Main();
	}
}

void fillTriangle(UCHAR* matrix, int x1, int y1, int x2, int y2, int x3, int y3, 
				  int tile_width, int tile_height)
{
	int new_x[3] = {x1, x2, x3};
	int new_y[3] = {y1, y2, y3};

	int temp_x, temp_y;
	int in;

	for (int i = 1; i < 3; i++)
	{
		temp_x = new_x[i];
		temp_y = new_y[i];
		for (in = i; in > 0 && new_y[in-1] > temp_y; in--)
		{
			new_y[in] = new_y[in-1];
			new_x[in] = new_x[in-1];
		}
		new_x[in] = temp_x;
		new_y[in] = temp_y;
	}

	double left_ratio = double(new_x[1] - new_x[0]) / double(new_y[1] - new_y[0]);
	double right_ratio = double(new_x[2] - new_x[0]) / double(new_y[2] - new_y[0]);

	double temp;

	if (left_ratio > right_ratio)
	{
		temp = left_ratio;
		left_ratio = right_ratio;
		right_ratio = temp;
	}

	double start_x;
	double end_x = start_x = double(new_x[0]);

	UCHAR* dest_ptr;// = tile_width * new_y[0];

	for (int y = new_y[0]; y < new_y[1]; y++)
	{
		dest_ptr = matrix + y * tile_width * TILEWIDTH + int(start_x);

		for (i = int(start_x); i < int(end_x); i++)
		{
			*dest_ptr = 255^(*dest_ptr);
			dest_ptr++;
		}

		start_x += left_ratio;
		end_x += right_ratio;
	}

	if (new_x[2] > new_x[1])
	{
		left_ratio = double(new_x[2] - new_x[1]) / double(new_y[2] - new_y[1]);
	}
	else
	{
		right_ratio = double(new_x[2] - new_x[1]) / double(new_y[2] - new_y[1]);
	}

	for (y = new_y[1]; y < new_y[2]; y++)
	{
		dest_ptr = matrix + y * tile_width * TILEWIDTH + int(start_x);

		for (i = int(start_x); i < int(end_x); i++)
		{
			*dest_ptr = 255^(*dest_ptr);

			dest_ptr++;
		}

		start_x += left_ratio;
		end_x += right_ratio;
	}
}
// stores the general structure of the hard tiles used for filling in tile boxes
UCHAR tile_hard_data[25][2] = 
{
// 775
{4, 4},
{4, 4},
{4, 4},
{4, 4},

// 779
{3, 3},

// 780
{1, 3},
{3, 2},
{3, 1},
{2, 3},

// 784
{0, 1},
{2, 0},
{1, 0},
{0, 2},

// 788
{4, 4},
{4, 4},
{4, 4},
{4, 4},

// 792
{4, 4},
{4, 4},
{4, 4},
{4, 4},

// 796
{1, 1},
{2, 2},
{3, 0},
{0, 3}};

void HardTileSelector::placeTileBox()
{
	if (tile_box_points <= 2)
		return;

	RECT bounds = {tile_box_x[0], tile_box_y[0], tile_box_x[0], tile_box_y[0]};

	// calculate bounds of polygon
	for (int i = 0; i < MAX_TILE_BOX_POINTS; i++)
	{
		if (tile_box_x[i] < bounds.left)
			bounds.left = tile_box_x[i];
		if (tile_box_x[i] > bounds.right)
			bounds.right = tile_box_x[i];
		if (tile_box_y[i] < bounds.top)
			bounds.top = tile_box_y[i];
		if (tile_box_y[i] > bounds.bottom)
			bounds.bottom = tile_box_y[i];
	}

	// calculate location of possible hard tiles
	RECT tiles;

	tiles.left = ((bounds.left + current_map->window.left) / (SCREEN_WIDTH + screen_gap)) * SCREEN_TILE_WIDTH +
		((bounds.left + current_map->window.left) % (SCREEN_WIDTH + screen_gap)) / TILEWIDTH;

	tiles.right = ((bounds.right + current_map->window.left) / (SCREEN_WIDTH + screen_gap)) * SCREEN_TILE_WIDTH +
		((bounds.right + current_map->window.left) % (SCREEN_WIDTH + screen_gap)) / TILEWIDTH;

	tiles.top = ((bounds.top + current_map->window.top) / (SCREEN_HEIGHT + screen_gap)) * SCREEN_TILE_HEIGHT +
		((bounds.top + current_map->window.top) % (SCREEN_HEIGHT + screen_gap)) / TILEHEIGHT; 

	tiles.bottom = ((bounds.bottom + current_map->window.top) / (SCREEN_HEIGHT + screen_gap)) * SCREEN_TILE_HEIGHT +
		((bounds.bottom + current_map->window.top) % (SCREEN_HEIGHT + screen_gap)) / TILEHEIGHT; 

	int x_tiles = tiles.right - tiles.left + 1;
	int y_tiles = tiles.bottom - tiles.top + 1;

	// reset the location of the triangles coordinates to match the matrix
	int cut_x = (tiles.left / SCREEN_TILE_WIDTH) * (SCREEN_WIDTH + screen_gap) + 
		(tiles.left % SCREEN_TILE_WIDTH) * TILEWIDTH -  current_map->window.left;

	int cut_y = (tiles.top / SCREEN_TILE_HEIGHT) * (SCREEN_HEIGHT + screen_gap) + 
		(tiles.top % SCREEN_TILE_HEIGHT) * TILEHEIGHT - current_map->window.top;

	for (i = 0; i < MAX_TILE_BOX_POINTS; i++)
	{
		tile_box_x[i] -= cut_x;
		tile_box_y[i] -= cut_y;
	}

	struct HARD_TILE
	{
		UCHAR hard[TILEHEIGHT][TILEWIDTH];
	};

	struct MINI_HARD_TILE
	{
		short hard[2][2];
	};

	UCHAR* matrix = new UCHAR[y_tiles * TILEHEIGHT * x_tiles * TILEWIDTH];
	memset(matrix, 0, y_tiles * TILEHEIGHT * x_tiles * TILEWIDTH);

	for (i = 2; i < MAX_TILE_BOX_POINTS; i++)
	{
		fillTriangle(matrix, tile_box_x[0], tile_box_y[0],
			tile_box_x[i - 1], tile_box_y[i - 1], tile_box_x[i], tile_box_y[i], x_tiles, y_tiles);
	}

	MINI_HARD_TILE* mini_matrix = new MINI_HARD_TILE[y_tiles * x_tiles];

	memset(mini_matrix, 0, y_tiles * x_tiles * sizeof(MINI_HARD_TILE));

	short* dest_mini = mini_matrix[0].hard[0];

	HARD_TILE temp_tile;

	UCHAR *tmp_tile_ptr;

	for (int y = 0; y < y_tiles; y++)
	{
		for (int x = 0; x < x_tiles; x++)
		{
			tmp_tile_ptr = temp_tile.hard[0];

			memset(tmp_tile_ptr, 0, TILEWIDTH * TILEHEIGHT);

			for (i = 0; i < TILEHEIGHT; i++)
			{
				memcpy(tmp_tile_ptr, matrix + (y * TILEHEIGHT + i) * x_tiles * TILEWIDTH + x * TILEWIDTH, TILEWIDTH);
				tmp_tile_ptr += TILEWIDTH;
			}

			tmp_tile_ptr = temp_tile.hard[0];

			for (int y2 = 0; y2 < TILEHEIGHT; y2++)
			{
				for (int x2 = 0; x2 < TILEWIDTH; x2++)
				{
					if (*tmp_tile_ptr)
						mini_matrix[y * x_tiles + x].hard[y2 / (TILEHEIGHT / 2)][x2 / (TILEWIDTH / 2)]++;

					tmp_tile_ptr++;
				}
			}
		}
	}

	delete [] matrix;

	int current_screen;

	UCHAR new_tile[2];

	// create an action for this command
	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_HARD_TILE_MULTIPLE_CHANGE;
	action->hard_tile_multiple_change.first_tile_x = tiles.left;
	action->hard_tile_multiple_change.first_tile_y = tiles.top;
	int width = action->hard_tile_multiple_change.width = tiles.right - tiles.left + 1;
	int height = action->hard_tile_multiple_change.height = tiles.bottom - tiles.top + 1;

	action->hard_tile_multiple_change.old_hard_tiles = new int[width * height];
	action->hard_tile_multiple_change.new_hard_tiles = new int[width * height];

	memset(action->hard_tile_multiple_change.old_hard_tiles, 0, sizeof(int) * width * height);
	memset(action->hard_tile_multiple_change.new_hard_tiles, 0, sizeof(int) * width * height);

	// save the old hard tile information
	int m_y;
	for (m_y = tiles.top, y = 0; y < height; m_y++, y++)
	{
		for (int m_x = tiles.left, x = 0; x < width; m_x++, x++)
		{
			int cur_screen = MAP_COLUMNS * (m_y / SCREEN_TILE_HEIGHT) + m_x / SCREEN_TILE_WIDTH;
			if (current_map->screen[cur_screen] == NULL)
				continue;
			int x_tile = m_x % SCREEN_TILE_WIDTH;
			int y_tile = m_y % SCREEN_TILE_HEIGHT;

			action->hard_tile_multiple_change.old_hard_tiles[y * width + x] = current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness;
			action->hard_tile_multiple_change.new_hard_tiles[y * width + x] = current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness;
		}
	}

	// get the new hard tiles to use
	for (m_y = tiles.top, y = 0; y < height; m_y++, y++)
	{
		for (int m_x = tiles.left, x = 0; x < width; m_x++, x++)
		{
			current_screen = MAP_COLUMNS * (m_y / SCREEN_TILE_HEIGHT) + m_x / SCREEN_TILE_WIDTH;
			if (current_map->screen[current_screen] == NULL)
				continue;
			x_tile = m_x % SCREEN_TILE_WIDTH;
			y_tile = m_y % SCREEN_TILE_HEIGHT;

			memset(new_tile, 0, 2);

			for (int y3 = 0; y3 < 2; y3++)
			{
				for (int x3 = 0; x3 < 2; x3++)
				{
					if (mini_matrix[y * x_tiles + x].hard[y3][x3] > 150)
						new_tile[y3] += 2 >> x3; 
				}
			}

			for (i = 0; i < 25; i++)
			{
				if (memcmp(tile_hard_data[i], new_tile, 2) == 0)
				{
					action->hard_tile_multiple_change.new_hard_tiles[y * width + x] = 775 + i;
					break;
				}
			}
		}
	}

	tile_box_points = 0;

	current_map->undo_buffer->addUndo(action);

	delete [] mini_matrix;	
}

int HardTileSelector::updateScreenGrid(int x, int y)
{
	screen_first_x_tile = screen_last_x_tile = (x + current_map->window.left) / (SCREEN_WIDTH + screen_gap) * SCREEN_TILE_WIDTH
		+ ((x + current_map->window.left) % (SCREEN_WIDTH + screen_gap)) / TILEWIDTH;

	screen_first_y_tile = screen_last_y_tile = (y + current_map->window.top) / (SCREEN_HEIGHT + screen_gap) * SCREEN_TILE_HEIGHT
		+ ((y + current_map->window.top) % (SCREEN_HEIGHT + screen_gap)) / TILEHEIGHT; 

	anchorRight = false;
	anchorBottom = false;

	return true;
}

int HardTileSelector::updateScreenGrid(RECT &mouse_box)
{
	screen_first_x_tile = ((mouse_box.left + current_map->window.left) / (SCREEN_WIDTH + screen_gap)) * SCREEN_TILE_WIDTH +
		((mouse_box.left + current_map->window.left) % (SCREEN_WIDTH + screen_gap)) / TILEWIDTH;

	screen_last_x_tile = ((mouse_box.right + current_map->window.left) / (SCREEN_WIDTH + screen_gap)) * SCREEN_TILE_WIDTH +
		((mouse_box.right + current_map->window.left) % (SCREEN_WIDTH + screen_gap)) / TILEWIDTH;

	screen_first_y_tile = ((mouse_box.top + current_map->window.top) / (SCREEN_HEIGHT + screen_gap)) * SCREEN_TILE_HEIGHT +
		((mouse_box.top + current_map->window.top) % (SCREEN_HEIGHT + screen_gap)) / TILEHEIGHT; 

	screen_last_y_tile = ((mouse_box.bottom + current_map->window.top) / (SCREEN_HEIGHT + screen_gap)) * SCREEN_TILE_HEIGHT +
		((mouse_box.bottom + current_map->window.top) % (SCREEN_HEIGHT + screen_gap)) / TILEHEIGHT; 

	// reorder the tiles so they fit
	int temp;
	anchorRight = false;
	anchorBottom = false;
	if (screen_first_x_tile > screen_last_x_tile)
	{
		anchorRight = true;
		temp = screen_first_x_tile;
		screen_first_x_tile = screen_last_x_tile;
		screen_last_x_tile = temp;
	}
	if (screen_first_y_tile > screen_last_y_tile)
	{
		anchorBottom = true;
		temp = screen_first_y_tile;
		screen_first_y_tile = screen_last_y_tile;
		screen_last_y_tile = temp;
	}

	int x_tiles = (screen_last_x_tile - screen_first_x_tile + 1) - SCREEN_TILE_WIDTH * 2;
	int y_tiles = (screen_last_y_tile - screen_first_y_tile + 1) - SCREEN_TILE_HEIGHT * 2;
	if (x_tiles > 0)
	{
		if (anchorRight)
			screen_first_x_tile += x_tiles;
		else
			screen_last_x_tile -= x_tiles;
	}

	if (y_tiles > 0)
	{
		if (anchorBottom)
			screen_first_y_tile += y_tiles;
		else
			screen_last_y_tile -= y_tiles;
	}

	return true;
}

int HardTileSelector::screenRender()
{
	if (current_map->screenMode != HARDBOX_MODE)
		return false;

	// calculate possible screens to draw
	int first_x_screen = current_map->window.left / (SCREEN_WIDTH + screen_gap);
	int last_x_screen = current_map->window.right / (SCREEN_WIDTH + screen_gap);
	int first_y_screen = current_map->window.top / (SCREEN_HEIGHT + screen_gap);
	int last_y_screen = current_map->window.bottom / (SCREEN_HEIGHT + screen_gap);

	int screen;
	int x_offset = -(current_map->window.left % (SCREEN_WIDTH + screen_gap));
	int y_offset = -(current_map->window.top % (SCREEN_HEIGHT + screen_gap));

	int x_screen_offset = 0;
	int y_screen_offset;

	int screen_first_x_tile_in = screen_first_x_tile;
	int screen_last_x_tile_in = screen_last_x_tile;
	int screen_first_y_tile_in = screen_first_y_tile;
	int screen_last_y_tile_in = screen_last_y_tile;

	int x_diff = screen_last_x_tile - screen_first_x_tile + 1 - x_tiles_selected;
	if (x_diff > 0)
	{
		if (anchorRight)
			screen_first_x_tile_in += x_diff;
		else
			screen_last_x_tile_in -= x_diff;
	}
	
	int y_diff = screen_last_y_tile_in - screen_first_y_tile_in + 1 - y_tiles_selected;
	if (y_diff > 0)
	{
		if (anchorBottom)
			screen_first_y_tile_in += y_diff;
		else
			screen_last_y_tile_in -= y_diff;
	}

	for (int x_screen = first_x_screen; x_screen <= last_x_screen; x_screen++)
	{
		y_screen_offset = 0;
		for (int y_screen = first_y_screen; y_screen <= last_y_screen; y_screen++)
		{
			screen = y_screen * MAP_COLUMNS + x_screen;
			if (current_map->screen[screen] != NULL)
			{
				int x_offset2 = x_offset + x_screen_offset;
				int y_offset2 = y_offset + y_screen_offset;
				
				// now print grid
				int cur_x_tile, cur_y_tile;
				for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
				{
					for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
					{
						cur_x_tile = x_screen * SCREEN_TILE_WIDTH + x;
						cur_y_tile = y_screen * SCREEN_TILE_HEIGHT + y;

						RECT dest = {x * TILEWIDTH + x_offset2, y * TILEHEIGHT + y_offset2, 
							(x + 1) * TILEWIDTH + x_offset2, (y + 1) * TILEHEIGHT + y_offset2};

						// check if tile is in boundaries
						if (screen_first_x_tile_in <= cur_x_tile && screen_last_x_tile_in >= cur_x_tile && 
							screen_first_y_tile_in <= cur_y_tile && screen_last_y_tile_in >= cur_y_tile)
						{
							// draw the grid box around the tile
							drawBox(dest, color_tile_grid_in_boundary, 1);
						}
						// tile must be out of boundary
						else if (screen_first_x_tile <= cur_x_tile && screen_last_x_tile >= cur_x_tile && 
							screen_first_y_tile <= cur_y_tile && screen_last_y_tile >= cur_y_tile)
						{
							// draw the grid box around the tile
							drawBox(dest, color_tile_grid_out_boundary, 1);
						}
					}
				}

			}
			y_screen_offset += (SCREEN_HEIGHT + screen_gap);
		}
		x_screen_offset += (SCREEN_WIDTH + screen_gap);
	}

	return true;
}

int HardTileSelector::placeTiles()
{
	//is there any tiles selected?
	if (screen_first_x_tile == -1)
	{
		MessageBox(NULL, "You must click on one or more tile destinations before tile(s) can be placed.", "Cannot Place Tile(s)", MB_OK);
		return false;
	}
	
	//used if not all the tiles are used from 0,0
	int x_diff = 0, y_diff = 0;

	//is there tiles on the right not being used?
	if (anchorRight)
	{
		//how many?
		x_diff = x_tiles_selected - (screen_last_x_tile - screen_first_x_tile + 1);

		//over flowed, too many used, lets bring it down to max (0)
		if (x_diff < 0)
		{
			screen_first_x_tile -= x_diff;
			x_diff = 0;
		}
	}
	//is there tiles on the top not being used?
	if (anchorBottom)
	{
		//how many?
		y_diff = y_tiles_selected - (screen_last_y_tile - screen_first_y_tile + 1);

		//over flowed, too many used, lets bring it down to max (0)
		if (y_diff < 0)
		{
			screen_first_y_tile -= y_diff;
			y_diff = 0;
		}	
	}

	//lets get the demensions of what the usee wants to paste
	//make sure we don't go out of bounds
	int width = screen_last_x_tile - screen_first_x_tile + 1;

	if (width > x_tiles_selected - x_diff)
		width = x_tiles_selected - x_diff;

	int height = screen_last_y_tile - screen_first_y_tile + 1;
	if (height > y_tiles_selected - y_diff)
		height = y_tiles_selected - y_diff;

	//add to undo buffer
	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_HARD_TILE_CHANGE;
	action->hard_tile_change.first_tile_x = screen_first_x_tile;
	action->hard_tile_change.first_tile_y = screen_first_y_tile;
	action->hard_tile_change.width = width;
	action->hard_tile_change.height = height;

	int cur_screen, x_tile, y_tile, x, y;

	// save the new tiles
	for (x = 0; x < width; x++)
	{
		for(y = 0; y < height; y++)
		{
			action->hard_tile_change.new_hard_tiles[x][y] = hard_tile_set[x+x_diff][y+y_diff];
		}
	}

	//save old tiles
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			//check screen we could have moved onto a new screen, or we could still be on it.
			cur_screen = MAP_COLUMNS * ((y + screen_first_y_tile) / SCREEN_TILE_HEIGHT) + (x + screen_first_x_tile) / SCREEN_TILE_WIDTH;

			//make sure the screen is valid
			if (current_map->screen[cur_screen] == NULL)
				continue;
			
			//zoom into the tile in question
			x_tile = (x + screen_first_x_tile) % SCREEN_TILE_WIDTH;
			y_tile = (y + screen_first_y_tile) % SCREEN_TILE_HEIGHT;

			//copy it over
			if (current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness)
			{
				action->hard_tile_change.old_hard_tiles[x][y] = current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness;
			} else {
				TILEDATA tempdata = current_map->screen[cur_screen]->tiles[y_tile][x_tile];

				action->hard_tile_change.old_hard_tiles[x][y] = tileBmp[tempdata.bmp]->tile_hardness[tempdata.y][tempdata.x];
			}
		}
	}

	current_map->undo_buffer->addUndo(action);

	return true;
}

int HardTileSelector::storeScreenTiles() 
{  
	if (screen_first_x_tile == -1) 
	{ 
		MessageBox(NULL, "You must click on one or more tile destinations before tile(s) can be copied.", "Cannot Place Tile(s)", MB_OK); 
		return false;  
	} 
	x_tiles_selected = 0; 
	y_tiles_selected; 
	int cur_screen, x_tile, y_tile;  

	for (int x = screen_first_x_tile; x <= screen_last_x_tile; x++, x_tiles_selected++) 
	{ 
		for (int y = screen_first_y_tile, y_tiles_selected = 0; y <= screen_last_y_tile; y++, y_tiles_selected++)  
		{ 
			cur_screen = MAP_COLUMNS * (y / SCREEN_TILE_HEIGHT) + x / SCREEN_TILE_WIDTH; 

			//if the screen is null (not used), we can skip it and map the hardness NULL
			if (current_map->screen[cur_screen] == NULL) 
			{ 
				hard_tile_set[x_tiles_selected][y_tiles_selected] = 0;
			} 
			else 
			{ 
				x_tile = x % SCREEN_TILE_WIDTH;  
				y_tile = y % SCREEN_TILE_HEIGHT; 
				if (current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness)
				{
					//use the alternate hardness
					hard_tile_set[x_tiles_selected][y_tiles_selected] = current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness;
				} else {
					//no alternate hardness, use default
					//get the tile information (bmp and tile x,y for the TS#)
					TILEDATA tempdata = current_map->screen[cur_screen]->tiles[y_tile][x_tile];

					hard_tile_set[x_tiles_selected][y_tiles_selected] = tileBmp[tempdata.bmp]->tile_hardness[tempdata.y][tempdata.x];
				}
			} 
		} 
	} 
 
	x_tiles_selected = screen_last_x_tile - screen_first_x_tile + 1; 
	y_tiles_selected = screen_last_y_tile - screen_first_y_tile + 1; 
 
	return true; 
}