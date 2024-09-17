#ifndef _TILE_H_
#define _TILE_H_

#include <ddraw.h>
#include "Globals.h"

// DEFINES ////////////////////////////////////////////////

// GLOBALS ////////////////////////////////////////////////



// PROTOTYPES /////////////////////////////////////////////

struct GRAPHICQUE;

class Tile
{
public:
	Tile(int file_source);
	~Tile();

	int loadSurface();

	int render(RECT dest_rect, int tileX, int tileY, LPDIRECTDRAWSURFACE7 &surface);
	int renderAll(int x, int y, int width, int height);
	
	int width, height;  // size of bitmap
	int file_location;

	LPDIRECTDRAWSURFACE7 image;
	
	// stores the tile hardness id that is associated with each square
	int tile_hardness[SCREEN_TILE_HEIGHT][SCREEN_TILE_WIDTH];

private:
	GRAPHICQUE* graphic_que_location;	// so we don't run out of memory
};


#endif