#ifndef _HARD_TILE_SELECTOR_H_
#define _HARD_TILE_SELECTOR_H_

#define HARD_TILE_SELECTOR_BMP_GAP		4
#define MAX_TILE_BOX_POINTS				4

#include "Globals.h"
#include "Screen.h"

struct GRAPHICQUE;

class HardTileSelector
{
public:
	HardTileSelector();
	~HardTileSelector();
	int render();
	int drawGrid(int x, int y);
	int drawHardGrid(int, int);
	int getTile(int x, int y);
	int renderHardness(int x, int y, int &hard_tile_num, bool transparent);
	int renderHardness(int hard_tile_num, bool transparent);
	int loadSurface(int hard_tile_num);
	int unloadSurface(int hard_tile_num);
	int updateScreenGrid(int x, int y);
	int updateScreenGrid(RECT &mouse_box);
	int revertTile();
	int placeTiles();
	int storeScreenTiles();
	int renderScreenTileHover(int x, int y);
	int nextPage();
	int prevPage();
	int resizeScreen();
	int screenRender();
	int LoadEditor();
	int displayEditor(int x, int y);
	int SaveEditor();
	int DrawEditor();
	void TransformHardTile(int hardness);
	void placeTileBoxPoint(int x, int y);
	void placeTileBox();
	void unloadAllBitmaps();
	void SetDefaultHardTile();

	UCHAR hardTile[NUM_HARD_TILES][TILEWIDTH][TILEHEIGHT];

	int hard_tile_set[SCREEN_TILE_WIDTH * 2][SCREEN_TILE_HEIGHT * 2];

	int tile_box_points;

private:
	int selectorScreen;
	int cur_screen;
	int x_tile, y_tile;
	int box_width, box_height;
	
	int tile_box_x[MAX_TILE_BOX_POINTS], tile_box_y[MAX_TILE_BOX_POINTS];

	int tiles_per_row, tiles_per_column;
	int pics_displayed;
	int max_pics;

	// grid for screenmode
	int x_tiles_selected;
	int y_tiles_selected;
	int screen_first_x_tile, screen_last_x_tile, screen_first_y_tile, screen_last_y_tile;

	int cur_hard_num, drag_hardness;
	TILEDATA cur_tile;
	UCHAR current_hardness[50][50];

	bool anchorRight, anchorBottom;

	LPDIRECTDRAWSURFACE7	hardTileSurface[NUM_HARD_TILES];
	GRAPHICQUE*				graphic_que_location[NUM_HARD_TILES];	// so we don't run out of memory
	TILEDATA tile;
};

#endif