#ifndef _MAP_H_
#define _MAP_H_

#include "Globals.h"
#include "HardTileSelector.h"
#include "SpriteSelector.h"
#include "TileSelector.h"
#include "GraphicsManager.h"
#include "SpriteLibrary.h"
#include "Structs.h"
#include "Sprite.h"
#include "sequence.h"

#define MAX_MAP_X (MAP_COLUMNS * (SCREEN_WIDTH + screen_gap) - screen_gap)
#define MAX_MAP_Y (MAP_ROWS * (SCREEN_HEIGHT + screen_gap) - screen_gap)

class Minimap;
class Screen;
class HardTileSelector;
class HardTileEditor;
class SpriteSelector;
class TileSelector;
class GraphicsManager;
class SpriteLibrary;
class Undo;
struct UNDO_TILES_CHANGE;
class Sequence;
class ImportMap;

class Map
{
public:
	Map();
	Map(CString dmod_path);
	~Map();

	int drawScreens();
	int placeSprite(int x, int y, bool stamp);
	int pickupSprite();
	Sprite* getSpriteClick(int &sprite_num);
	void updateMapPosition(int x, int y);
	void updateHoverPosition(int x, int y);
	void finishLoading();
	void deleteScreen(int screen_num);
	void newScreen(int screen_num);
	void pasteScreen(int screen_num, Screen* source_screen, int midi_num, int indoor);
	void changeScreenProperties(int screen_num, const char* script, int midi_num, int indoor);
	void changeSpriteProperties(int screen_num, int sprite_num, Sprite* new_sprite, Sprite* old_sprite);
	void setVision(int new_vision);
	void loadMouseSprite(Sprite* sprite, int screen_num);
	void setMouseSprite(Sprite* sprite);
	Sprite* getMouseSprite() { return mouseSprite; }

	void undoAction();
	void redoAction();

	void placeTiles(int first_x_tile, int first_y_tile, int width, int height, 
					 TILEDATA tile_set[SCREEN_TILE_WIDTH * 2][SCREEN_TILE_HEIGHT * 2]);

	void trimSprite(RECT trim);

	void screenshot(char* filename);

	void drawScreen(int my_screen);

	void toggleAutoUpdateMinimap();

	// io functions
	int open_dmod();
	int parse_dink_ini();
	int parse_dink_dat();
	int parse_map_dat(int screen_order[NUM_SCREENS]);
	int parse_hard_dat();
	int read_tile_bmps();

	int save_dmod();
	int save_dink_dat();
	int save_map_dat();
	int save_dink_ini();
	int save_hard_dat();

	int deallocate_sequences();
	int deallocate_map();
	int release_tile_bmps();

	// end io functions


	Screen* screen[NUM_SCREENS];
	HardTileSelector hard_tile_selector;
	SpriteSelector sprite_selector;
	TileSelector tile_selector;
	SpriteLibrary sprite_library;
	Undo* undo_buffer;

	Sequence* sequence[MAX_SEQUENCES];

	int midi_num[NUM_SCREENS];
	int indoor[NUM_SCREENS];
	bool miniupdated[NUM_SCREENS];

	// coordinates of window in relation to position on map
	RECT window;
	int window_width;
	int window_height;
	int cur_vision;
	int cur_screen;

	void lock_screen() {if (screen[screen_hover] != NULL) locked_screen = screen_hover;}
	void unlock_screen() {locked_screen = -1;}

	CString dmod_path;	// pathname to dmod directory
	CString import_dmod_path;

	DDBLTFX	blankscreen;	// used for drawing a blank screen

	Minimap* minimap;

	int screen_hover, x_hover, y_hover;	// x and y coordinates of mouse in relation to current screen
	int screen_x_hover, screen_y_hover;	// gives the row and column of the screen
	int screen_x_offset, screen_y_offset;	// how many pixels off the upper left screen is offset
	Sprite* hover_sprite;
	int hover_sprite_num;
	RECT screens_displayed;

	bool screenmatch;
	int render_state;
	int screenMode;
	Sprite* mouseSprite;

	ImportMap* import_map;

private:
	int locked_screen;
	// calculate possible screens to draw
};

#endif