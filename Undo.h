#ifndef _UNDO_H_
#define _UNDO_H_

#include "Globals.h"

#include "Structs.h"
#include <deque>
#include <vector>
using namespace std;

// undo types
#define UT_SCREEN_CREATE				1
#define UT_SCREEN_PASTE					2
#define UT_SCREEN_DELETE				3
#define UT_SCREEN_PROPERTIES_CHANGE		4
#define UT_SCREEN_IMPORT				5

#define UT_SPRITE_PICKUP				7
#define UT_SPRITE_PLACE					8
#define UT_SPRITE_DELETE				9
#define UT_SPRITE_CREATE				10
#define UT_SPRITE_PROPERTIES_CHANGE		11

#define UT_TILES_CHANGE					13
#define UT_HARD_TILE_CHANGE				14
#define UT_HARD_TILE_MULTIPLE_CHANGE	15
#define UT_TRANSFORM_HARD_TILE			16
#define UT_SET_DEFAULT_HARD_TILE		17
#define UT_HARD_TILE_EDIT				18

class Screen;
class Sprite;

struct UNDO_HARD_TILE_EDIT
{
	int hard_tile_num;
	UCHAR old_tile_data[50][50];
	UCHAR new_tile_data[50][50];
};

struct UNDO_TRANSFORM_HARD_TILE
{
	int tile_num;
	int old_tile_data[50][50];
	short hardness;
};

struct UNDO_SET_DEFAULT_HARD_TILE
{
	int old_index;
	int new_index;
	int bmp;
	int x,y;
};

struct UNDO_HARD_TILE_MULTIPLE_CHANGE
{
	int width;
	int height;
	int first_tile_x, first_tile_y;
	int *new_hard_tiles;
	int *old_hard_tiles;
};

struct UNDO_SCREEN_CREATE
{
	int screen;
};

struct UNDO_SCREEN_PASTE
{
	int screen_num;
	Screen* new_screen;
	int midi_num;
	int indoor;
};

struct UNDO_SCREEN_DELETE
{
	int screen_num;
	Screen* old_screen;
	int midi_num;
	int indoor;
};

struct UNDO_SCREEN_PROPERTIES_CHANGE
{
	int screen_num;
	char old_script[BASE_SCRIPT_MAX_LENGTH], new_script[BASE_SCRIPT_MAX_LENGTH];
	int old_midi, new_midi;
	int old_indoor, new_indoor;
};

struct UNDO_SCREEN_IMPORT
{
	int screen_num;
	Screen* new_screen;
	int midi_num;
	int indoor;
};

struct UNDO_SPRITE_PICKUP
{
	int screen_num;
	int sprite_num;
	Sprite *new_mouse_sprite;
};

struct UNDO_SPRITE_PLACE
{
	vector<int> *screen_num;
	vector<int> *sprite_num;
	vector<Sprite*> *sprite_list;
	bool stamp;
};

struct UNDO_SPRITE_DELETE
{
	Sprite *mouse_sprite;
};

struct UNDO_SPRITE_CREATE
{
	Sprite *old_mouse_sprite;
	Sprite *new_mouse_sprite;
};

struct UNDO_SPRITE_PROPERTY_CHANGE
{
	Sprite *old_sprite, *new_sprite;
	int screen_num;
	int sprite_num;
};

struct UNDO_TILES_CHANGE
{
	int first_x_tile, first_y_tile;
	int width, height;
	TILEDATA new_tiles[SCREEN_TILE_WIDTH * 2][SCREEN_TILE_HEIGHT * 2];
	TILEDATA old_tiles[SCREEN_TILE_WIDTH * 2][SCREEN_TILE_HEIGHT * 2];
};

struct UNDO_HARD_TILE_CHANGE
{
	int screen_num;
	int width;
	int height;
	int first_tile_x, first_tile_y;
	int new_hard_tiles[SCREEN_TILE_WIDTH * 2][SCREEN_TILE_HEIGHT * 2];
	int old_hard_tiles[SCREEN_TILE_WIDTH * 2][SCREEN_TILE_HEIGHT * 2];
};

struct UNDO_ACTION
{
	int type;
	union
	{
		UNDO_SCREEN_CREATE					screen_create;
		UNDO_SCREEN_PASTE					screen_paste;
		UNDO_SCREEN_DELETE					screen_delete;
		UNDO_SCREEN_PROPERTIES_CHANGE		screen_properties_change;
		UNDO_SCREEN_IMPORT					screen_import;
		UNDO_SPRITE_PICKUP					sprite_pickup;
		UNDO_SPRITE_PLACE					sprite_place;
		UNDO_SPRITE_DELETE					sprite_delete;
		UNDO_SPRITE_CREATE					sprite_create;
		UNDO_SPRITE_PROPERTY_CHANGE			sprite_property_change;
		UNDO_TILES_CHANGE					tiles_change;
		UNDO_HARD_TILE_CHANGE				hard_tile_change;
		UNDO_HARD_TILE_MULTIPLE_CHANGE		hard_tile_multiple_change;
		UNDO_TRANSFORM_HARD_TILE			transform_hard_tile;
		UNDO_SET_DEFAULT_HARD_TILE			set_default_hard_tile;
		UNDO_HARD_TILE_EDIT					hard_tile_edit;
	};
	int x_coord, y_coord;
	int vision;
};

// DEFINES ////////////////////////////////////////////////

// GLOBALS ////////////////////////////////////////////////



// PROTOTYPES /////////////////////////////////////////////

class Undo
{
public:
	Undo();
	~Undo();

	void undoAction();
	void redoAction();
	void doAction(UNDO_ACTION* action);
	void addUndo(UNDO_ACTION* undo_action);
	void popBack(deque<UNDO_ACTION*>* action_list);

private:
	deque<UNDO_ACTION*> undo_list;
	deque<UNDO_ACTION*> redo_list;

	bool overflowed_undo_stack;
};


#endif