#ifndef _TILE_SELECTOR_H_
#define _TILE_SELECTOR_H_

#include <ddraw.h>

#include "Globals.h"
#include "Screen.h"

class TileSelector
{
public:
	TileSelector();
	~TileSelector();

	int updateGrid(int x, int y);
	int updateGrid(int mouse_x_origin, int mouse_x_position, int mouse_y_origin, int mouse_y_position);
	int updateScreenGrid(int x, int y);
	int updateScreenGrid(RECT &mouse_box);
	int render();
	int screenRender();
	int storeTiles();
	int storeScreenTiles();
	int placeTiles();
	int selectScreen(int new_screen);
	int renderTileHover(int x, int y);
	int renderScreenTileHover(int x, int y);

private:

	// grid for tilemode
	int first_x_tile, last_x_tile, first_y_tile, last_y_tile;

	// current tile bitmap
	int current_tile;

	// stores tiles in a buffer for copying and pasting
	TILEDATA tile_set[SCREEN_TILE_WIDTH * 2][SCREEN_TILE_HEIGHT * 2];
	int x_tiles_selected;
	int y_tiles_selected;

	// grid for screenmode
	int screen_first_x_tile, screen_last_x_tile, screen_first_y_tile, screen_last_y_tile;

	bool anchorRight, anchorBottom;
};

#endif