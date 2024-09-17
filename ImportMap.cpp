#include <StdAfx.h>

#include <fstream.h>
#include <string.h>

#include "ImportMap.h"
#include "Globals.h"
#include "Screen.h"
#include "Engine.h"
#include "Map.h"
#include "Minimap.h"
#include "Interface.h"
#include "Sprite.h"
#include "undo.h"

/////////////////// Functions

void removeImportMap()
{
	if (current_map->import_map != NULL)
	{
		delete current_map->import_map;
		current_map->import_map = NULL;
	}
	// show screen
	current_map->minimap->drawSquares();
	current_map->render_state = SHOW_MINIMAP;
	Game_Main();
}

ImportMap::ImportMap()
{
	src_dmod_path = current_map->import_dmod_path;

	loadMap(src_dmod_path);
	source_tile = -1;
	dest_tile = -1;
}

ImportMap::~ImportMap()
{

}

int ImportMap::loadMap(CString pathname)
{
	FILE *stream;

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", pathname, "dink.dat");

	if ((stream = fopen(filename, "rb")) == NULL)
		return false;

	fseek(stream, 24, SEEK_SET);

	// read in the order the screens were created
	// this lets us know where in the map.dat each screen is located
	fread(screen_order, sizeof(int) * NUM_SCREENS, 1, stream);

	fseek(stream, 4, SEEK_CUR);

	// read in midi numbers for each screen
	fread(midi_num, sizeof(int) * NUM_SCREENS, 1, stream);

	fseek(stream, 4, SEEK_CUR);

	// read in whether the screen is indoor or outdoor
	fread(indoor, sizeof(int) * NUM_SCREENS, 1, stream);

	fclose(stream);

	current_map->minimap->renderImportMap(screen_order, midi_num, indoor);

	return true;
}

// a screen was selected
void ImportMap::tileClicked()
{
	if (source_tile == -1)
	{
		if (screen_order[source_tile] == 0)
			return;

		source_tile = current_map->minimap->hover_screen;

		current_map->minimap->drawSquares();
		Game_Main();
	}
	else
	{
		// load the new screen
		dest_tile = current_map->minimap->hover_screen;

		if (current_map->screen[dest_tile] != NULL)
		{
			MessageBox(NULL, "You cannot place a screen on an existing one, you must delete the target screen first", "Error", MB_OK);
			return;
		}
	
		importScreen(src_dmod_path, source_tile, dest_tile);

		current_map->minimap->drawSquares();
		
		removeImportMap();
	}
}

// draw the import map
bool ImportMap::render()
{
	current_map->minimap->render();

	return true;
}

// import the selected screen to the destination screen
int ImportMap::importScreen(CString pathname, int src_screen, int dest_screen)
{
	FILE *stream;

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", pathname, "map.dat");

	if ((stream = fopen(filename, "rb")) == NULL)
		return false;

	// find the next screen to load
	int map_location = screen_order[src_screen] - 1;
	if (map_location == -1)
	{
		return false;
	}

	// go to that spot in the map file to load
	fseek(stream, map_location * SCREEN_DATA_SIZE, SEEK_SET);

	// advance to first tile
	fseek(stream, 20, SEEK_CUR);

	// if old screen doesn't exist, create a new one
	if (current_map->screen[dest_screen] != NULL)
	{
		delete current_map->screen[dest_screen];
		current_map->screen[dest_screen] = NULL;
	}

	current_map->screen[dest_screen] = new Screen;

	int temp;

	DUMMY_TILE tile_data[SCREEN_TILE_HEIGHT][SCREEN_TILE_WIDTH];

	// read in the tile data
	fread(tile_data, sizeof(DUMMY_TILE) * SCREEN_TILE_HEIGHT * SCREEN_TILE_WIDTH, 1, stream);
	
	for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
		{
			temp = tile_data[y][x].tile;
			// make the map format easier to read
			current_map->screen[dest_screen]->tiles[y][x].bmp = short((temp >> 7) & ((1 << 9) - 1));
			current_map->screen[dest_screen]->tiles[y][x].x = short((temp & ((1 << 7) - 1)) % 12);
			current_map->screen[dest_screen]->tiles[y][x].y = short((temp & ((1 << 7) - 1)) / 12);

			current_map->screen[dest_screen]->tiles[y][x].alt_hardness = tile_data[y][x].alt_hardness;
		}
	}
	
	// advance to sprite data
	fseek(stream, 540, SEEK_CUR);

	Sprite* new_sprite;

	memset(current_map->screen[dest_screen]->sprite, 0 , MAX_SPRITES * sizeof(int));

	DUMMY_SPRITE sprite_data[MAX_SPRITES];

	// read in a sprite block
	if (fread(sprite_data, sizeof(DUMMY_SPRITE) * MAX_SPRITES, 1, stream) == 0)
		return false;

	int next_sprite = 0;

	// read in the sprite data
	for (int cur_sprite = 0; cur_sprite < MAX_SPRITES; cur_sprite++)
	{
		// check if there's anything in the sprite block
		if (sprite_data[cur_sprite].sprite_exist == true)
		{
			// found a sprite, now allocate the structure for it
			new_sprite = new Sprite(sprite_data[cur_sprite]);

			current_map->screen[dest_screen]->sprite[next_sprite] = new_sprite;
			next_sprite++;
		}
	}

	fclose(stream);

	// copy over the midi number and indoor status
	current_map->midi_num[dest_screen] = midi_num[src_screen];
	current_map->indoor[dest_screen] = indoor[src_screen];

	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_SCREEN_IMPORT;
	action->screen_import.new_screen = current_map->screen[dest_screen];
	action->screen_import.midi_num = midi_num[src_screen];
	action->screen_import.indoor = indoor[src_screen];
	action->screen_import.screen_num = dest_screen;

	current_map->undo_buffer->addUndo(action);

	return true;
}


