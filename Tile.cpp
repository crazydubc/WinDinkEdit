#include "StdAfx.h"

#include "Tile.h"
#include "Globals.h"
#include "ddutil.h"
#include "Map.h"
#include "Engine.h"
#include "GraphicsManager.h"
#include "Colors.h"
#include "Common.h"

Tile::Tile(int file_source)
{
	file_location = file_source;
	image = NULL;
}

Tile::~Tile()
{
	if (image)
	{
		image->Release();
		image = NULL;
	}
}

int Tile::loadSurface()
{
	char bmp_file[MAX_PATH];
	
	wsprintf(bmp_file, "%sTiles\\Ts%.2d.bmp", current_map->dmod_path, file_location);
	
	if ((image = DDLoadBitmap(bmp_file, width, height)) == NULL)
	{
		// try loading from the dink dmod directory
		wsprintf(bmp_file, "%sTiles\\Ts%.2d.bmp", dink_path, file_location);
		if ((image = DDLoadBitmap(bmp_file, width, height)) == NULL)
			return false;
	}

	graphic_que_location = graphics_manager.addGraphic(&image);

	return true;
}

// render the entire tile bitmap
int Tile::renderAll(int x, int y, int dest_width, int dest_height)
{
	// draw a bob at the x,y defined in the BOB
	// on the destination surface defined in dest
	if (image == NULL)
		loadSurface();

	if (image == NULL)
		return false;

	RECT dest_rect;                        

	// fill in the destination rect
	dest_rect.left   = x;
	dest_rect.top    = y;
	dest_rect.right  = x + int(double(dest_width) * (double(width) / double(SCREEN_WIDTH)));
	dest_rect.bottom = y + int(double(dest_height) * (double(height) / double(SCREEN_HEIGHT)));

	// blt to destination surface
	HRESULT hr = lpddsback->Blt(&dest_rect, image, NULL, DDBLT_WAIT, NULL);

	graphics_manager.moveLocation(graphic_que_location);

	if(hr == DDERR_SURFACELOST)
	{
		if(image->IsLost())
		{
			image->Restore();
			loadSurface();
		}
	}

	return true;
} // end render

int Tile::render(RECT dest_rect, int tileX, int tileY, LPDIRECTDRAWSURFACE7 &surface)
{
	// draw a bob at the x,y defined in the BOB
	// on the destination surface defined in dest
	if (image == NULL)
		loadSurface();

	RECT source_rect;

	// fill in the source rect
	source_rect.left    = tileX * TILEWIDTH;
	source_rect.top     = tileY * TILEHEIGHT;
	source_rect.right   = source_rect.left + TILEWIDTH;
	source_rect.bottom  = source_rect.top + TILEHEIGHT;

	// trim tile
	RECT clip_rect = {0, 0, current_map->window_width, current_map->window_height};

	if (fixBounds(dest_rect, source_rect, clip_rect) == false)
		return false;

	// blt to destination surface
	HRESULT hr = surface->Blt(&dest_rect, image, &source_rect, DDBLT_WAIT, NULL);

	graphics_manager.moveLocation(graphic_que_location);

	if(hr == DDERR_SURFACELOST)
	{
		if(image->IsLost())
		{
			image->Restore();
			loadSurface();
		}
	}

	return true;
}