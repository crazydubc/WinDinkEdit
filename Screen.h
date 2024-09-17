#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "Globals.h"
#include "Structs.h"

class Sprite;

class Screen
{
public:
	Screen();
	Screen(Screen* copy_screen);
	~Screen();

	Sprite* getSprite(int x, int y, int &sprite_num);
	int addSprite(int sprite_num, Sprite *new_sprite);
	void removeSprite(int sprite_num);
	int findFirstAvailableSpriteSlot();
	int drawTiles(int x_offset, int y_offset, LPDIRECTDRAWSURFACE7 &surface);
	int drawSprites(int x_offset, int y_offset, RECT &clip_rect, LPDIRECTDRAWSURFACE7 &surface);
	int drawHardTiles(int x_offset, int y_offset);
	int drawHardSprites(int x_offset, int y_offset, RECT &clip_rect);

	TILEDATA tiles[SCREEN_TILE_HEIGHT][SCREEN_TILE_WIDTH];
	Sprite* sprite[MAX_SPRITES];

	char script[BASE_SCRIPT_MAX_LENGTH];
};

#endif