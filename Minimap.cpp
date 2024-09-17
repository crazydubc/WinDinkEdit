#include "StdAfx.h"

#include <ddraw.h>

#include "Minimap.h"
#include "Globals.h"
#include "ddutil.h"
#include "Map.h"
#include "Engine.h"
#include "Screen.h"
#include "MainFrm.h"
#include "Colors.h"
#include "Common.h"

Minimap::Minimap()
{
	image = NULL;
	mapSquareImage[0] = NULL;
	mapSquareImage[1] = NULL;
	mapSquareImage[2] = NULL;
	mapSquareImage[3] = NULL;

	memset(&grid_color,0,sizeof(DDBLTFX));
	grid_color.dwSize = sizeof(DDBLTFX);
	grid_color.dwFillColor = 0;

	loadSurface();

	hover_x_screen = 0;
	hover_y_screen = 0;
}


Minimap::~Minimap()
{
	if (image != NULL)
		image->Release();

	for (int i = 0; i < 4; i++)
	{
		if (mapSquareImage[i] != NULL)
			mapSquareImage[i]->Release();
	}
}

int Minimap::unloadSurface()
{
	if (image != NULL)
		image->Release();
	return true;
}

int Minimap::reloadSurface()
{
	if (image->IsLost())
		image->Restore();

	loadSurface();
	render();
	return true;
}

// loads the 
int Minimap::loadSquare(char *filename, int square_num)
{
	if (mapSquareImage[square_num] != NULL)
		return false;

	// load the map squares
	char bmp_file[MAX_PATH];

	int dummy = 0;
	
	wsprintf(bmp_file, "%sTiles\\%s", current_map->dmod_path, filename);
	if ((mapSquareImage[square_num] = DDLoadBitmap(bmp_file, dummy, dummy)) == NULL)
	{
		// try loading from the dink dmod directory
		wsprintf(bmp_file, "%sTiles\\%s", dink_path, filename);
		if ((mapSquareImage[square_num] = DDLoadBitmap(bmp_file, dummy, dummy)) == NULL)
			return false;
	}

	return true;
}

int Minimap::restoreSurfaces()
{
	if(image)
		if(image->IsLost())
			image->Restore();

	if(mapSquareImage[SQUARE_EMPTY])
	{
		if(mapSquareImage[SQUARE_EMPTY]->IsLost())
		{
			mapSquareImage[SQUARE_EMPTY]->Restore();
			loadSquare(SQUARE_EMPTY_NAME, SQUARE_EMPTY);
		}
	}

	if(mapSquareImage[SQUARE_USED])
	{
		if(mapSquareImage[SQUARE_USED]->IsLost())
		{
			mapSquareImage[SQUARE_USED]->Restore();
			loadSquare(SQUARE_USED_NAME, SQUARE_USED);
		}
	}

	if(mapSquareImage[SQUARE_MIDI])
	{
		if(mapSquareImage[SQUARE_MIDI]->IsLost())
		{
			mapSquareImage[SQUARE_MIDI]->Restore();
			loadSquare(SQUARE_MIDI_NAME, SQUARE_MIDI);
		}
	}

	if(mapSquareImage[SQUARE_INDOOR])
	{
		if(mapSquareImage[SQUARE_INDOOR]->IsLost())
		{
			mapSquareImage[SQUARE_INDOOR]->Restore();
			loadSquare(SQUARE_INDOOR_NAME, SQUARE_INDOOR);
		}
	}
		
	drawMap();

	return true;
}

int Minimap::loadSurface()
{
	DDSURFACEDESC2 ddsd; // create the surface description

	// set to access caps, width, and height
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// set dimensions of the new bitmap surface
	ddsd.dwWidth  = MINI_MAP_WIDTH;		// width in pixels, not memory size
	ddsd.dwHeight = MINI_MAP_HEIGHT;
	
	image = graphics_manager.allocateSurface(&ddsd);

	loadSquare(SQUARE_EMPTY_NAME, SQUARE_EMPTY);
	loadSquare(SQUARE_USED_NAME, SQUARE_USED);
	loadSquare(SQUARE_MIDI_NAME, SQUARE_MIDI);
	loadSquare(SQUARE_INDOOR_NAME, SQUARE_INDOOR);

	// set color key to black
	DDCOLORKEY color_key; // used to set color key
	color_key.dwColorSpaceLowValue  = 0;
	color_key.dwColorSpaceHighValue = 0;

	// now set the color key for source blitting
	mapSquareImage[SQUARE_MIDI]->SetColorKey(DDCKEY_SRCBLT, &color_key);
	mapSquareImage[SQUARE_INDOOR]->SetColorKey(DDCKEY_SRCBLT, &color_key);

	drawSquares();

	return true;
}

// draws the entire minimap
int Minimap::drawMap()
{
	RECT src_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	RECT dst_rect;

	dst_rect.top = 0;
	dst_rect.bottom = SQUARE_HEIGHT;

	int cur_screen = 0;
	char buffer[50];

	// make clip box for use when rendering each screen
	RECT clip_box = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	mainWnd->m_wndStatusBar.SetRange(1,768);

	for (int y = 0; y < MAP_ROWS; y++)
	{
		if (KEY_DOWN(VK_ESCAPE))
			return true;

		dst_rect.left = 0;
		dst_rect.right = SQUARE_WIDTH;
		for (int x = 0; x < MAP_COLUMNS; x++, cur_screen++)
		{
			int temp = double(double(double(cur_screen) / (MAP_ROWS * MAP_COLUMNS)) * 100);

			//skip if it doesn't need to be updated and if the option is enabled
			if (!current_map->miniupdated[cur_screen] || !fast_minimap_update)
			{
				// fills the back buffer with black
				if (current_map->screen[cur_screen] != NULL)
				{
					lpddsback->Blt(&src_rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &grid_color);
					current_map->screen[cur_screen]->drawTiles(0, 0, lpddsback);
					current_map->screen[cur_screen]->drawSprites(0, 0, clip_box, lpddsback);
					image->Blt(&dst_rect, lpddsback, &src_rect, DDBLT_WAIT, NULL);
	
					// show progress is active
					if (show_minimap_progress)
					{
						lpddsback->Blt(NULL, current_map->minimap->image, NULL, DDBLT_WAIT, 0);
						lpddsprimary->Blt(&mapRect, lpddsback, NULL, DDBLT_WAIT, 0);
					}
				}
				else
				{
					image->Blt(&dst_rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &grid_color);
				}

				current_map->miniupdated[cur_screen] = true;
			}
			dst_rect.left += SQUARE_WIDTH;
			dst_rect.right += SQUARE_WIDTH;

			mainWnd->m_wndStatusBar.OnProgress(cur_screen);
		}
		dst_rect.top += SQUARE_HEIGHT;
		dst_rect.bottom += SQUARE_HEIGHT;
	}
	mainWnd->setStatusText("Finished Minimap Detailing");
	return 1;
}

// draws purple or red tile on minimap for target screen
int Minimap::renderMapSquare(int cur_screen)
{
	int x_square = cur_screen % MAP_COLUMNS;
	int y_square = cur_screen / MAP_COLUMNS;

	RECT dest = {0, 0, SQUARE_WIDTH, 0};
	dest.left = x_square * SQUARE_WIDTH;
	dest.top = y_square * SQUARE_HEIGHT;
	dest.right = dest.left + SQUARE_WIDTH;
	dest.bottom = dest.top + SQUARE_HEIGHT;

	// draw blank screen tile
	if (current_map->screen[cur_screen] == NULL)
	{
		image->Blt(&dest, mapSquareImage[SQUARE_EMPTY], NULL, DDBLT_WAIT, NULL);
	}	
	else
	{
		// draw non-blank screen tile
		image->Blt(&dest, mapSquareImage[SQUARE_USED], NULL, DDBLT_WAIT, NULL);
		if (current_map->midi_num[cur_screen] != 0)
			image->Blt(&dest, mapSquareImage[SQUARE_MIDI], NULL, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);
		if (current_map->indoor[cur_screen] == 1)
			image->Blt(&dest, mapSquareImage[SQUARE_INDOOR], NULL, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);
	}

	return true;
}

// draws purple and red tiles for entire minimap
int Minimap::drawSquares()
{
	RECT dest = {0, 0, SQUARE_WIDTH, SQUARE_HEIGHT};
	int cur_screen = 0;

	for (int y = 0; y < MAP_ROWS; y++)
	{
		dest.right = SQUARE_WIDTH;
		dest.left = 0;
		for (int x = 0; x < MAP_COLUMNS; x++)
		{
			// draw blank screen tile
			if (current_map->screen[cur_screen] == NULL)
			{
				image->Blt(&dest, mapSquareImage[SQUARE_EMPTY], NULL, DDBLT_WAIT, NULL);
			}
			// draw non-blank screen tile
			else
			{
				image->Blt(&dest, mapSquareImage[SQUARE_USED], NULL, DDBLT_WAIT, NULL);
				if (current_map->midi_num[cur_screen] != 0)
					image->Blt(&dest, mapSquareImage[SQUARE_MIDI], NULL, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);
				if (current_map->indoor[cur_screen] == 1)
					image->Blt(&dest, mapSquareImage[SQUARE_INDOOR], NULL, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);
			}

			cur_screen++;

			dest.left += SQUARE_WIDTH;
			dest.right += SQUARE_WIDTH;
		}
		dest.bottom += SQUARE_HEIGHT;
		dest.top += SQUARE_HEIGHT;
	}

	return true;
}

// used when importing a screen from another map
int Minimap::renderImportMap(int screen_order[], int midi_num[], int indoor[])
{
	int cur_screen = 0;

	RECT dest = {0, 0, SQUARE_WIDTH, SQUARE_HEIGHT};
	for (int y = 0; y < MAP_ROWS; y++)
	{
		dest.right = SQUARE_WIDTH;
		dest.left = 0;
		for (int x = 0; x < MAP_COLUMNS; x++)
		{
			if (screen_order[cur_screen] == 0)
			{
				image->Blt(&dest, mapSquareImage[SQUARE_EMPTY], NULL, DDBLT_WAIT, NULL);
			}
			else
			{
				image->Blt(&dest, mapSquareImage[SQUARE_USED], NULL, DDBLT_WAIT, NULL);
				if (midi_num[cur_screen] != 0)
					image->Blt(&dest, mapSquareImage[SQUARE_MIDI], NULL, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);
				if (indoor[cur_screen] == 1)
					image->Blt(&dest, mapSquareImage[SQUARE_INDOOR], NULL, (DDBLT_WAIT | DDBLT_KEYSRC), NULL);

			}

			dest.left += SQUARE_WIDTH;
			dest.right += SQUARE_WIDTH;

			cur_screen++;
		}
		dest.bottom += SQUARE_HEIGHT;
		dest.top += SQUARE_HEIGHT;
	}

	return true;
}

// draws the minimap onto the screen
bool Minimap::render()
{
	lpddsback->Blt(NULL, image, NULL, DDBLT_WAIT, 0);
	drawGrid();

	return true;
}

// draws a box around the currently highlighted screen
int Minimap::drawGrid()
{
	RECT dest = {int(x_scale * double(hover_x_screen)), int(y_scale * double(hover_y_screen)), 
		int(x_scale * double(hover_x_screen + 1)), int(y_scale * double(hover_y_screen + 1))};
	drawBox(dest, color_minimap_screen_selected, 1);

	return true;
}

void Minimap::updateHoverPosition(int x, int y)
{
// displays screen coordinates of mouse pointer
	hover_x_screen = int(double(x) / x_scale);
	hover_y_screen = int(double(y) / y_scale);
	hover_screen = hover_y_screen * MAP_COLUMNS + hover_x_screen;
	mainWnd->updateMousePos(hover_screen + 1, 0, 0);
}

// calculates the x and y location of the screen
void Minimap::loadScreen()
{
	int x_location = hover_x_screen * (SCREEN_WIDTH + screen_gap) + SCREEN_WIDTH / 2 - (current_map->window_width / 2);
	int y_location = hover_y_screen * (SCREEN_HEIGHT + screen_gap) + SCREEN_HEIGHT / 2 - (current_map->window_height / 2);

	current_map->updateMapPosition(x_location, y_location);

	current_map->render_state = SHOW_SCREEN;
	Game_Main();
}

void Minimap::resizeScreen()
{
	x_scale = double(SQUARE_WIDTH) * double(current_map->window_width) / double(MINI_MAP_WIDTH);
	y_scale = double(SQUARE_HEIGHT) * double(current_map->window_height) / double(MINI_MAP_HEIGHT);
}