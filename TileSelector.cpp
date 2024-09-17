#include "StdAfx.h"

#include <ddraw.h>

#include "TileSelector.h"
#include "Tile.h"
#include "Map.h"
#include "Globals.h"
#include "ddutil.h"
#include "Engine.h"
#include "Screen.h"
#include "Colors.h"
#include "Common.h"
#include "Undo.h"

TileSelector::TileSelector()
{
	x_tiles_selected = -1;
	y_tiles_selected = -1;
	screen_first_x_tile = -1;
	screen_last_x_tile = -1;
	screen_first_y_tile = -1;
	screen_last_y_tile = -1;
}

TileSelector::~TileSelector()
{

}

int TileSelector::selectScreen(int new_screen)
{
	current_tile = 4 * new_screen;

	current_map->render_state = SHOW_TILE_SELECTOR;
	current_map->screenMode = TILE_MODE;

/*
	first_x_tile = 0;
	last_x_tile = 0;
	first_y_tile = 0;
	last_y_tile = 0;*/

	return true;
}

int TileSelector::updateGrid(int x, int y)
{
	// calculate width of one entire tile bmp
	int width = current_map->window_width / 2 - TILE_SELECTOR_GAP / 2;
	int height = current_map->window_height / 2 - TILE_SELECTOR_GAP / 2;

	// now draw the grid on the selected tiles
	double tile_width = double(width) / SCREEN_TILE_WIDTH;
	double tile_height = double(height) / SCREEN_TILE_HEIGHT;

	int new_x_tile = int(x / tile_width);
	int new_y_tile = int(y / tile_height);

	first_x_tile = last_x_tile = new_x_tile;
	first_y_tile = last_y_tile = new_y_tile;

	return true;
}

// calculates a grid from a rect that contains the currently selected tiles
int TileSelector::updateGrid(int mouse_x_origin, int mouse_x_position, int mouse_y_origin, int mouse_y_position)
{
	// calculate width of one entire tile bmp
	int width = current_map->window_width / 2 - TILE_SELECTOR_GAP / 2;
	int height = current_map->window_height / 2 - TILE_SELECTOR_GAP / 2;

	// now draw the grid on the selected tiles
	double tile_width = double(width) / SCREEN_TILE_WIDTH;
	double tile_height = double(height) / SCREEN_TILE_HEIGHT;

	// set left
	if (mouse_x_origin <= width)
		first_x_tile = int(mouse_x_origin / tile_width);
	else
		first_x_tile = int((mouse_x_origin - TILE_SELECTOR_GAP) / tile_width);

	// set top
	if (mouse_x_position <= width)
		last_x_tile = int(mouse_x_position / tile_width);
	else
		last_x_tile = int((mouse_x_position - TILE_SELECTOR_GAP) / tile_width);

	// set right
	if (mouse_y_origin <= height)
		first_y_tile = int(mouse_y_origin / tile_height);
	else
		first_y_tile = int((mouse_y_origin - TILE_SELECTOR_GAP) / tile_height);

	// set bottom
	if (mouse_y_position <= height)
		last_y_tile = int(mouse_y_position / tile_height);
	else
		last_y_tile = int((mouse_y_position - TILE_SELECTOR_GAP) / tile_height);

	// reorder the tiles so they fit
	int temp;
	if (first_x_tile > last_x_tile)
	{
		// swap left with right
		temp = first_x_tile;
		first_x_tile = last_x_tile;
		last_x_tile = temp;
	}
	if (first_y_tile > last_y_tile)
	{
		// swap top with bottom
		temp = first_y_tile;
		first_y_tile = last_y_tile;
		last_y_tile = temp;
	}

	return true;
}

int TileSelector::updateScreenGrid(int x, int y)
{
	screen_first_x_tile = screen_last_x_tile = (x + current_map->window.left) / (SCREEN_WIDTH + screen_gap) * SCREEN_TILE_WIDTH
		+ ((x + current_map->window.left) % (SCREEN_WIDTH + screen_gap)) / TILEWIDTH;

	screen_first_y_tile = screen_last_y_tile = (y + current_map->window.top) / (SCREEN_HEIGHT + screen_gap) * SCREEN_TILE_HEIGHT
		+ ((y + current_map->window.top) % (SCREEN_HEIGHT + screen_gap)) / TILEHEIGHT; 

	anchorRight = false;
	anchorBottom = false;

	return true;
}

int TileSelector::updateScreenGrid(RECT &mouse_box)
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

// stores tiles on tile bmp screens
int TileSelector::storeTiles()
{
	x_tiles_selected = 0;
	y_tiles_selected;

	int xBmp, yBmp;
	for (int x = first_x_tile; x <= last_x_tile; x++, x_tiles_selected++)
	{
		for (int y = first_y_tile, y_tiles_selected = 0; y <= last_y_tile; y++, y_tiles_selected++)
		{
			xBmp = x / SCREEN_TILE_WIDTH;
			yBmp = y / SCREEN_TILE_HEIGHT;
			tile_set[x_tiles_selected][y_tiles_selected].bmp = yBmp * 2 + xBmp + current_tile;
			tile_set[x_tiles_selected][y_tiles_selected].x = x - xBmp * SCREEN_TILE_WIDTH;
			tile_set[x_tiles_selected][y_tiles_selected].y = y - yBmp * SCREEN_TILE_HEIGHT;
			tile_set[x_tiles_selected][y_tiles_selected].alt_hardness = 0;
		}
	}

	x_tiles_selected = last_x_tile - first_x_tile + 1;
	y_tiles_selected = last_y_tile - first_y_tile + 1;

	return true;
}

// stores tiles from the map screen
int TileSelector::storeScreenTiles() 
{  
	if (screen_first_x_tile == -1) 
	{ 
		MessageBox(NULL, "You must click on one or more tile destinations before tile(s) can be copied.", "Cannot Place Tile(s)", MB_OK); 
		return false;  
	} 
	x_tiles_selected = 0; 
	y_tiles_selected; 
//	TILEDATA* current_tile; 
	int cur_screen, x_tile, y_tile;  

	for (int x = screen_first_x_tile; x <= screen_last_x_tile; x++, x_tiles_selected++) 
	{ 
		for (int y = screen_first_y_tile, y_tiles_selected = 0; y <= screen_last_y_tile; y++, y_tiles_selected++)  
		{ 
			cur_screen = MAP_COLUMNS * (y / SCREEN_TILE_HEIGHT) + x / SCREEN_TILE_WIDTH; 
			if (current_map->screen[cur_screen] == NULL) 
			{ 
				memset(&tile_set[x_tiles_selected][y_tiles_selected], 0, sizeof(TILEDATA)); 
			} 
			else 
			{ 
				x_tile = x % SCREEN_TILE_WIDTH;  
				y_tile = y % SCREEN_TILE_HEIGHT; 
				memcpy(&tile_set[x_tiles_selected][y_tiles_selected], &current_map->screen[cur_screen]->tiles[y_tile][x_tile], sizeof(TILEDATA)); 
				//tile_set[x_tiles_selected][y_tiles_selected].alt_hardness = 0; 
			} 
		} 
	} 
 
	x_tiles_selected = screen_last_x_tile - screen_first_x_tile + 1; 
	y_tiles_selected = screen_last_y_tile - screen_first_y_tile + 1; 
 
	return true; 
}

int TileSelector::placeTiles()
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
	action->type = UT_TILES_CHANGE;
	action->tiles_change.first_x_tile = screen_first_x_tile;
	action->tiles_change.first_y_tile = screen_first_y_tile;
	action->tiles_change.width = width;
	action->tiles_change.height = height;

	int cur_screen, x_tile, y_tile, x, y;

	// save the new tiles
	for (x = 0; x < width; x++)
	{
		for(y = 0; y < height; y++)
		{
			action->tiles_change.new_tiles[x][y] = tile_set[x+x_diff][y+y_diff];
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
			action->tiles_change.old_tiles[x][y] = current_map->screen[cur_screen]->tiles[y_tile][x_tile];
		}
	}

	current_map->undo_buffer->addUndo(action);

	return true;
}

// draws tile grid for selecting tiles, does not draw the actual tiles
int TileSelector::screenRender()
{
	if (current_map->screenMode != TILE_MODE)
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

int TileSelector::render()
{
	// draw the tile bmps
	int width = current_map->window_width / 2 - TILE_SELECTOR_GAP / 2;
	int height = current_map->window_height / 2 - TILE_SELECTOR_GAP / 2;

	tileBmp[current_tile]->renderAll(0, 0, width, height);

	if (current_tile != 40)
	{
		tileBmp[current_tile + 1]->renderAll(width + TILE_SELECTOR_GAP, 0, width, height);
		tileBmp[current_tile + 2]->renderAll(0, height + TILE_SELECTOR_GAP, width, height);
		tileBmp[current_tile + 3]->renderAll(width + TILE_SELECTOR_GAP, height + TILE_SELECTOR_GAP, width, height);
	}
	// now draw the grid on the selected tiles
	double tile_width = double(width) / SCREEN_TILE_WIDTH;
	double tile_height = double(height) / SCREEN_TILE_HEIGHT;

	// draw tiles
	int x_offset, y_offset;
	for (double x = first_x_tile; x <= last_x_tile; x++)
	{
		for (double y = first_y_tile; y <= last_y_tile; y++)
		{
			if (int(x) >= SCREEN_TILE_WIDTH)
				x_offset = 5;
			else
				x_offset = 0;
			if (int(y) >= SCREEN_TILE_HEIGHT)
				y_offset = 5;
			else
				y_offset = 0;

			RECT dest = {int(x * tile_width) + x_offset, int(y * tile_height) + y_offset, 
				int((x + 1) * tile_width) + x_offset, int((y + 1) * tile_height) + y_offset};
			drawBox(dest, color_tile_grid, 1);
		}
	}

	return true;
}

int TileSelector::renderTileHover(int x, int y)
{
	// find the widths of the tiles
	int tilebmp_width = current_map->window_width / 2 - TILE_SELECTOR_GAP / 2;
	int tilebmp_height = current_map->window_height / 2 - TILE_SELECTOR_GAP / 2;

	// now draw the grid on the selected tiles
	double tile_width = double(tilebmp_width) / double(SCREEN_TILE_WIDTH);
	double tile_height = double(tilebmp_height) / double(SCREEN_TILE_HEIGHT);

	int x_tile = int(double(x) / tile_width);
	int y_tile = int(double(y) / tile_height);

	// draw tiles
	int x_offset, y_offset;

	if (int(x_tile) >= SCREEN_TILE_WIDTH)
		x_offset = 5;
	else
		x_offset = 0;
	if (int(y_tile) >= SCREEN_TILE_HEIGHT)
		y_offset = 5;
	else
		y_offset = 0;

	int x_location = int(double(x_tile) * tile_width) + x_offset;
	int y_location = int(double(y_tile) * tile_height) + y_offset;

	RECT dest = {x_location, y_location, x_location + int(tile_width), y_location + int (tile_height)};
	drawBox(dest, color_tile_grid, 1);

	return true;
}

int TileSelector::renderScreenTileHover(int x, int y)
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

	return true;
}