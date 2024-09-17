// this is part of the Map class and includes the I/O functionality
#include "StdAfx.h"

#include <fstream>
using namespace std;

#include "Globals.h"
#include "Map.h"
#include "Engine.h"
#include "Screen.h"
#include "Sequence.h"
#include "Minimap.h"
#include "Tile.h"
#include "MainFrm.h"
#include "HardTileSelector.h"
#include "Structs.h"
#include "SpriteLibrary.h"

#define DINK_HEADER_NAME		"Smallwood"

struct DUMMY_HARD_TILE
{
	UCHAR data[TILEWIDTH][TILEHEIGHT + 1];	// 1 extra garbage bit
};

struct DUMMY_TILE_HARDNESS
{
	int data[SCREEN_TILE_HEIGHT][SCREEN_TILE_WIDTH];
	int garbage[32];
};

// FUNCTIONS /////////////////////////////////////

int Map::open_dmod()
{
	Game_Init();	// load up directdraw

	if (!parse_dink_dat())
	{
		return false;
	}
	if (!read_tile_bmps())
	{
		return false;
	}
	if (!parse_dink_ini())
	{
		return false;
	}
	if (!parse_hard_dat())
	{
		return false;
	}

	memset(&miniupdated, 0, sizeof(miniupdated));

	return true;
}

int Map::save_dmod()
{
	if(optimize_dink_ini)
		save_dink_ini();

	if (!save_dink_dat())
	{
		return false;
	}
	if (!save_map_dat())
	{
		return false;
	}
	sprite_library.storeList();
	if (!save_hard_dat())
	{
		return false;
	}

	return true;
}

int Map::deallocate_sequences()
{
	for (int i = 0; i < MAX_SEQUENCES; i++)
	{
		if (sequence[i])
		{
			delete sequence[i];
			sequence[i] = NULL;
		}
	}
	return true;
}

int Map::release_tile_bmps()
{
	for (int i = 0; i < MAX_TILE_BMPS; i++)
	{
		if (tileBmp[i])
		{
			delete tileBmp[i];
			tileBmp[i] = NULL;
		}
	}

	return true;
}

int Map::deallocate_map()
{
	for (int i = 0; i < NUM_SCREENS; i++)
	{
		if (screen[i])
		{
			delete screen[i];
			screen[i] = NULL;
		}
	}
	return true;
}

int Map::read_tile_bmps(void)
{
	for (int i = 0; i < MAX_TILE_BMPS; i++)
	{
		tileBmp[i] = new Tile(i + 1);
	}

	return true;
}

int Map::save_hard_dat(void)
{
	FILE *stream; 
  
	char filename[MAX_PATH]; 
	wsprintf(filename, "%s%s", dmod_path, "hard.dat"); 
 
	if ((stream = fopen(filename, "wb")) == NULL) 
	{ 
		MessageBox(NULL, "Could not open hard.dat", "Error", MB_OK);  
		return false; 
	} 

	UCHAR garbage[58]; 
	memset(garbage, 0, 58); 
 
	DUMMY_HARD_TILE hard_tile; 
 
	for (int i = 0; i < NUM_HARD_TILES; i++) 
	{ 
		// revert to the messed up file format 
		for (int y = 0; y < TILEHEIGHT; y++)  
		{ 
			for (int x = 0; x < TILEWIDTH; x++) 
			{ 
				hard_tile.data[x][y] = hard_tile_selector.hardTile[i][y][x]; 
			} 
		} 
 
		fwrite(&hard_tile, sizeof(DUMMY_HARD_TILE), 1, stream); 
		
		// write out useless data 
		fwrite(garbage, 58, 1, stream); 
	} 
 
	// each screen 96 blocks (12 x 8)  
	// 128 blocks 

	DUMMY_TILE_HARDNESS hard_tile_table[MAX_TILE_BMPS]; 

	for (i = 0; i < MAX_TILE_BMPS; i++) 
	{ 
		memcpy(&hard_tile_table[i], tileBmp[i]->tile_hardness, sizeof(int) * SCREEN_TILE_HEIGHT * SCREEN_TILE_WIDTH); 
	} 
 
	fwrite(&hard_tile_table, sizeof(DUMMY_TILE_HARDNESS) * MAX_TILE_BMPS, 1, stream); 

	fclose(stream); 
 
	return true; 
}

int Map::parse_hard_dat(void)
{
	/*
	hard.dat
	file size: 2,118,400
	800 entries
	block size of each tile: 2550
	size between blocks: 58
	first block begins at beginning of file
	column major order
	each pixel separated by 50 bytes of data followed by 1 byte = 0
	z is 1, normal hardness
	a is 2, water hardness
	s is 3, ??? hardness
	*/

	//UCHAR data[TILEWIDTH][TILEHEIGHT + 1];	// 1 extra garbage bit

	FILE *stream;

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", dmod_path, "hard.dat");

	if ((stream = fopen(filename, "rb")) == NULL)
	{
		MessageBox(NULL, "Could not open hard.dat", "Error", MB_OK);
		return false;
	}

	DUMMY_HARD_TILE hard_tile;

	UCHAR *dest_ptr;
	
	for (int i = 0; i < NUM_HARD_TILES; i++)
	{
		if(!fread(&hard_tile, sizeof(DUMMY_HARD_TILE), 1, stream))
			return false;

		dest_ptr = hard_tile_selector.hardTile[i][0];

		// fix the messed up file format
		for (int y = 0; y < TILEHEIGHT; y++)
		{
			for (int x = 0; x < TILEWIDTH; x++)
			{
				*dest_ptr = hard_tile.data[x][y];
				dest_ptr++;
			}
		}
		fseek(stream, 58, SEEK_CUR);	// skip useless data
	}

	// each screen 96 blocks (12 x 8)
	// 128 blocks
	DUMMY_TILE_HARDNESS hard_tile_table[MAX_TILE_BMPS];

	if(!fread(&hard_tile_table, sizeof(DUMMY_TILE_HARDNESS) * MAX_TILE_BMPS, 1, stream))
		return false;

	for (i = 0; i < MAX_TILE_BMPS; i++)
	{
		memcpy(tileBmp[i]->tile_hardness, &hard_tile_table[i], sizeof(int) * SCREEN_TILE_HEIGHT * SCREEN_TILE_WIDTH);
	}

	fclose(stream);

	return true;
}

int Map::parse_dink_dat(void)
{
	FILE *stream;
	int screen_order[NUM_SCREENS];

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", dmod_path, "dink.dat");

	if ((stream = fopen(filename, "rb")) == NULL)
	{
		// dink.dat not found, so clear the memory used by it
		memset(screen_order, 0, sizeof(int) * NUM_SCREENS);
		memset(midi_num, 0, sizeof(int) * NUM_SCREENS);
		memset(indoor, 0, sizeof(int) * NUM_SCREENS);

		// clear map.dat
		memset(screen, 0, sizeof(int) * NUM_SCREENS);

		return false;
	}

	fseek(stream, 24, SEEK_SET);

	// read in the order the screens were created
	// this lets us know where in the map.dat each screen is located
	if(!fread(screen_order, sizeof(int) * NUM_SCREENS, 1, stream))
		return false;

	fseek(stream, 4, SEEK_CUR);

	// read in midi numbers for each screen
	if (!fread(midi_num, sizeof(int) * NUM_SCREENS, 1, stream))
		return false;

	fseek(stream, 4, SEEK_CUR);

	// read in whether the screen is indoor or outdoor
	if(!fread(indoor, sizeof(int) * NUM_SCREENS, 1, stream))
		return false;

	fclose(stream);

	if (!parse_map_dat(screen_order))
	{
		return false;
	}

	return true;
}

int Map::parse_map_dat(int screen_order[NUM_SCREENS])
{
	FILE *stream;
	int i;

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", dmod_path, "map.dat");

	if ((stream = fopen(filename, "rb")) == NULL)
	{
		MessageBox(NULL, "Could not open map.dat", "Error", MB_OK);
		return false;
	}

	int map_location;
	Sprite* new_sprite;
	int temp;

	DUMMY_SPRITE sprite_data[MAX_SPRITES];
	DUMMY_TILE tile_data[SCREEN_TILE_HEIGHT][SCREEN_TILE_WIDTH];

	for (i = 0; i < NUM_SCREENS; i++)
	{
		// find the next screen to load
		map_location = screen_order[i] - 1;
		if (map_location == -1)
		{
			screen[i] = NULL;
			continue;
		}

		// go to that spot in the map file to load
		fseek(stream, map_location * SCREEN_DATA_SIZE, SEEK_SET);

		// advance to first tile
		fseek(stream, 20, SEEK_CUR);

		// read in the tiles
		screen[i] = new Screen;

		// read in the tile data
		if (!fread(tile_data, sizeof(DUMMY_TILE) * SCREEN_TILE_HEIGHT * SCREEN_TILE_WIDTH, 1, stream))
			return false;
		
		for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
		{
			for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
			{
				temp = tile_data[y][x].tile;
				// make the map format easier to read
				screen[i]->tiles[y][x].bmp = temp / (1 << 7);
				screen[i]->tiles[y][x].x = ((temp % (1 << 7)) % 12);
				screen[i]->tiles[y][x].y = (temp % (1 << 7)) / 12;

				screen[i]->tiles[y][x].alt_hardness = tile_data[y][x].alt_hardness;
			}
		}

		// advance to sprite data
		fseek(stream, 540, SEEK_CUR);

		// read in a sprite block
		if (fread(sprite_data, sizeof(DUMMY_SPRITE) * MAX_SPRITES, 1, stream) == 0)
			return false;

		//int next_sprite = 0; //cause problem with changing sprite numbers

		// read in the sprite data
		for (int cur_sprite = 0; cur_sprite < MAX_SPRITES; cur_sprite++)
		{
			// check if there's anything in the sprite block
			if (sprite_data[cur_sprite].sprite_exist == 1)
			{
				// found a sprite, now allocate the structure for it
				new_sprite = new Sprite(sprite_data[cur_sprite]);

				screen[i]->sprite[cur_sprite] = new_sprite;
				//next_sprite++;
			}
		}
		if(!fread(screen[i]->script, BASE_SCRIPT_MAX_LENGTH, 1, stream))
			return false;
	}

	fclose(stream);

	return true;
}

int Map::save_dink_ini()
{
	char buffer[MAX_PATH];
	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", dmod_path, "dink.ini");

	// open the dink.ini file
	ofstream dink_ini(filename);

	for (int cur_seq = 1; cur_seq <= MAX_SEQUENCES; cur_seq++)
	{
		if (current_map->sequence[cur_seq] != NULL)
		{
			if (current_map->sequence[cur_seq]->now == true)
			{
				//print the load_sequence_now
				dink_ini << "load_sequence_now " << 
					current_map->sequence[cur_seq]->graphics_path << " " <<
					cur_seq << " ";
			} else{
				dink_ini << "load_sequence " << 
					current_map->sequence[cur_seq]->graphics_path << " " <<
					cur_seq << " ";
			}

			//do we need to print all this?
			if (current_map->sequence[cur_seq]->frame_delay ||
				current_map->sequence[cur_seq]->center_x || 
				current_map->sequence[cur_seq]->center_y || 
				current_map->sequence[cur_seq]->left_boundary || 
				current_map->sequence[cur_seq]->top_boundary || 
				current_map->sequence[cur_seq]->right_boundary || 
				current_map->sequence[cur_seq]->bottom_boundary)
			{
				//yes we do...
				dink_ini << current_map->sequence[cur_seq]->frame_delay << " " <<
				current_map->sequence[cur_seq]->center_x << " " <<
				current_map->sequence[cur_seq]->center_y << " " <<
				current_map->sequence[cur_seq]->left_boundary << " " <<
				current_map->sequence[cur_seq]->top_boundary << " " <<
				current_map->sequence[cur_seq]->right_boundary << " " <<
				current_map->sequence[cur_seq]->bottom_boundary << " ";
			}

			//print special attrib's if needed
			if (current_map->sequence[cur_seq]->type == BLACK)
				dink_ini << "BLACK";

			else if (current_map->sequence[cur_seq]->type == NOTANIM)
				dink_ini << "NOTANIM";

			else if (current_map->sequence[cur_seq]->type == LEFTALIGN)
				dink_ini << "LEFTALIGN";

			dink_ini << "\n";
		}
	}

	dink_ini << "\n\n";

	for (cur_seq = 1; cur_seq < MAX_SEQUENCES; cur_seq++)
	{
		if (current_map->sequence[cur_seq] != NULL)
		{
			for (int cur_frame = 1; cur_frame < MAX_FRAMES; cur_frame++)
			{
				if (current_map->sequence[cur_seq]->frame_info[cur_frame-1] != NULL)
				{
					sprintf(buffer, "set_sprite_info %i %i %i %i %i %i %i %i", cur_seq, cur_frame, current_map->sequence[cur_seq]->frame_info[cur_frame-1]->center_x,
						current_map->sequence[cur_seq]->frame_info[cur_frame-1]->center_y,
						current_map->sequence[cur_seq]->frame_info[cur_frame-1]->left_boundary,
						current_map->sequence[cur_seq]->frame_info[cur_frame-1]->top_boundary,
						current_map->sequence[cur_seq]->frame_info[cur_frame-1]->right_boundary,
						current_map->sequence[cur_seq]->frame_info[cur_frame-1]->bottom_boundary);

					dink_ini << buffer << endl;
				}
			}
		}
	}

	dink_ini << "\n\n";

	for (cur_seq = 1; cur_seq < MAX_SEQUENCES; cur_seq++)
	{
		if (current_map->sequence[cur_seq] != NULL)
		{
			for (int cur_frame = 1; cur_frame < MAX_FRAMES; cur_frame++)
			{
				if (current_map->sequence[cur_seq]->set_frame_delay[cur_frame-1] != NULL)
				{
					sprintf(buffer, "set_frame_delay %i %i %i", cur_seq, cur_frame, current_map->sequence[cur_seq]->set_frame_delay[cur_frame-1]);
					dink_ini << buffer << endl;
				}
				if (current_map->sequence[cur_seq]->set_frame_special[cur_frame-1] != NULL)
				{
					sprintf(buffer, "set_frame_special %i %i %i", cur_seq, cur_frame, current_map->sequence[cur_seq]->set_frame_special[cur_frame-1]);
					dink_ini << buffer << endl;
				}
				if (current_map->sequence[cur_seq]->set_frame_frame_seq[cur_frame] != NULL)
				{
					sprintf(buffer, "set_frame_frame %i %i %i", cur_seq, cur_frame, sequence[cur_seq]->set_frame_frame_seq[cur_frame]);
					dink_ini << buffer;

					if (sequence[cur_seq]->set_frame_frame_seq[cur_frame] != -1)
						dink_ini << " " << sequence[cur_seq]->set_frame_frame_frame[cur_frame];

					dink_ini << endl;
				}
			}
		}
	}
	dink_ini.close();

	return true;
}

int Map::parse_dink_ini()
{
	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", dmod_path, "dink.ini");

	// open the dink.ini file
	ifstream dink_ini(filename);

	char input_line[MAX_LINE_LENGTH];
	char parse_inputs[MAX_PARSE_INPUTS][MAX_LINE_LENGTH];

	int num_tokens = 0;

	int num_special_frames = 0;

	while (dink_ini.getline(input_line, MAX_LINE_LENGTH))
	{
		// now let's process the text from this line

		// check for comments or blank lines and skip them
		if (input_line[0] == ';' || input_line[0] == '/' || strlen(input_line) == 0)
			continue;

		// parse line into up to MAX_PARSE_INPUTS strings
		char *token;
		token = strtok(input_line, " ,\t,\n");
		if (token == NULL)
			continue;
		strcpy(parse_inputs[0], token);
		
		for (num_tokens = 1; num_tokens < MAX_PARSE_INPUTS; num_tokens++)
		{
			token = strtok(NULL, " ,\t,\n");
			if (token == NULL)
				break;
			strcpy(parse_inputs[num_tokens], token);
		}

		if (num_tokens > 2)
		{
			// process command: load_sequence_now
			if (_stricmp("load_sequence_now", parse_inputs[0]) == 0 || _stricmp("load_sequence", parse_inputs[0]) == 0)
			{
				bool now;

				if (_stricmp("load_sequence_now", parse_inputs[0]) == 0)
					now = true;
				else
					now = false;

				int frame_delay = 0;
				int center_x = 0;
				int center_y = 0;
				int left_boundary = 0;
				int top_boundary = 0;
				int right_boundary = 0;
				int bottom_boundary = 0;
				int type = 0;

				int sequence_num = atoi(parse_inputs[2]);

				if (num_tokens >= 10)
				{
					frame_delay = atoi(parse_inputs[3]);
					center_x = atoi(parse_inputs[4]);
					center_y = atoi(parse_inputs[5]);
					left_boundary = atoi(parse_inputs[6]);
					top_boundary = atoi(parse_inputs[7]);
					right_boundary = atoi(parse_inputs[8]);
					bottom_boundary = atoi(parse_inputs[9]);
				//	type = NOTANIM;
				}
				else if (num_tokens != 3)
				{
					if (_stricmp("BLACK", parse_inputs[3]) == 0)
					{
						type = BLACK;
					}
					else if (_stricmp("NOTANIM", parse_inputs[3]) == 0)
					{
						type = NOTANIM;
					}
					else if (_stricmp("LEFTALIGN", parse_inputs[3]) == 0)
					{
						type = LEFTALIGN;
					}
					else
					{
						frame_delay = atoi(parse_inputs[3]);
					}
				}

				if (sequence[sequence_num] != NULL)
					continue;

				sequence[sequence_num] = new Sequence(sequence_num + 1, parse_inputs[1], frame_delay, type,
						center_x, center_y, left_boundary, top_boundary, right_boundary, bottom_boundary, now);
			}
			else if (_stricmp("set_sprite_info", parse_inputs[0]) == 0)
			{
				int cur_sequence = atoi(parse_inputs[1]);
				// check if sequence exists first
				if (sequence[cur_sequence] == NULL)
					continue;
				
				int frame_num = atoi(parse_inputs[2]) - 1;

				sequence[cur_sequence]->addFrameInfo(frame_num,
					atoi(parse_inputs[3]),
					atoi(parse_inputs[4]),
					atoi(parse_inputs[5]),
					atoi(parse_inputs[6]),
					atoi(parse_inputs[7]),
					atoi(parse_inputs[8]));

				num_special_frames++;
			}
			else if (_stricmp("set_frame_frame", parse_inputs[0]) == 0)
			{
				int dest_sequence = atoi(parse_inputs[1]);
				int dest_frame = atoi(parse_inputs[2]);
				int cur_sequence = atoi(parse_inputs[3]);
				int cur_frame = atoi(parse_inputs[4]);

				if (sequence[dest_sequence] == NULL)
					continue;

				sequence[dest_sequence]->set_frame_frame_seq[dest_frame] = cur_sequence;

				if (cur_sequence != -1)
					sequence[dest_sequence]->set_frame_frame_frame[dest_frame] = cur_frame;
			}
			else if (_stricmp("set_frame_delay", parse_inputs[0]) == 0)
			{
				int cur_sequence = atoi(parse_inputs[1]);
				int cur_frame = atoi(parse_inputs[2]);
				int delay = atoi(parse_inputs[3]);

				if (sequence[cur_sequence] == NULL)
					continue;

				sequence[cur_sequence]->set_frame_delay[cur_frame-1] = delay;
			}
			else if (_stricmp("set_frame_special", parse_inputs[0]) == 0)
			{
				int cur_sequence = atoi(parse_inputs[1]);
				int cur_frame = atoi(parse_inputs[2]);
				int hit = atoi(parse_inputs[3]);

				if (sequence[cur_sequence] == NULL)
					continue;

				sequence[cur_sequence]->set_frame_special[cur_frame-1] = hit;
			}
		}
	} // end while loop

	if (num_special_frames >= MAX_FRAME_INFOS)
	{
		char warning_message[1024];
		wsprintf(warning_message, "Note that you have exceeded the maximum number of \"set_sprite_info\" commands by %d. These will still work in WinDinkedit but will be ignored by Dinkedit and the Dink engine. Enable Optimize Dink.ini in the options menu and WDE will attempt to fix this on the next save.", num_special_frames - MAX_FRAME_INFOS + 1);
		MessageBox(NULL, warning_message, "Warning: Too many set_sprite_info commands", MB_OK);
	}

	return true;
}

int Map::save_dink_dat()
{
	FILE *stream;
	UCHAR dummy_array[2240];
	memset(dummy_array, 0, 2240);

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", dmod_path, "dink.dat");

	if ((stream = fopen(filename, "wb")) == NULL)
		return false;

	if(!fwrite(DINK_HEADER_NAME, sizeof(DINK_HEADER_NAME), 1, stream))
		return false;

	// write out garbage
	if(!fwrite(&dummy_array, 24 - sizeof(DINK_HEADER_NAME), 1, stream))
		return false;
	
	// get the order the screens were created
	int screen_order[NUM_SCREENS];
	
	// first fix the numbers so they are ordered
	int screen_count = 1;
	for (int i = 0; i < NUM_SCREENS; i++)
	{
		if (screen[i] != NULL)
		{
			screen_order[i] = screen_count;
			screen_count++;
		}
		else
		{
			screen_order[i] = 0;
		}	
	}

	// write out the order the screens were created
	if(!fwrite(&screen_order, sizeof(int) * NUM_SCREENS, 1, stream))
		return false;

	// write out garbage
	if(!fwrite(&dummy_array, 4, 1, stream))
		return false;

	// write out midi numbers for each screen
	if(!fwrite(midi_num, sizeof(int) * NUM_SCREENS, 1, stream))
		return false;

	// write out garbage
	if(!fwrite(&dummy_array, 4, 1, stream))
		return false;

	// write out whether the screen is indoor or outdoor
	if(!fwrite(indoor, sizeof(int) * NUM_SCREENS, 1, stream))
		return false;

	// write out extra garbage
	if(!fwrite(&dummy_array, 2240, 1, stream))
		return false;

	// close the file
	fclose(stream);

	return true;
}

int Map::save_map_dat()
{
	DUMMY_SPRITE sprite_data;

	UCHAR dummy_array[1024];
	memset(dummy_array, 0, 1024);

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", dmod_path, "map.dat");

	//DUMMY_SPRITE sprite_data[MAX_SPRITES];
	DUMMY_TILE tile_data[SCREEN_TILE_HEIGHT][SCREEN_TILE_WIDTH];
	memset(tile_data, 0, SCREEN_TILE_HEIGHT * SCREEN_TILE_WIDTH * sizeof(DUMMY_TILE));

	// open the map file
	FILE *stream;
	if ((stream = fopen(filename, "wb")) == NULL)
		return false;

	for (int i = 0; i < NUM_SCREENS; i++)
	{
		if (screen[i] == NULL)
			continue;

		// write out garbage
		if(!fwrite(&dummy_array, 20, 1, stream))
			return false;
	
		// fill in tile output matrix
		for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
		{
			for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
			{
				// tile number
				tile_data[y][x].tile = (screen[i]->tiles[y][x].bmp << 7) +
										screen[i]->tiles[y][x].x +
										screen[i]->tiles[y][x].y * 12;

				// tile hardness
				tile_data[y][x].alt_hardness = screen[i]->tiles[y][x].alt_hardness;
			}
		}

		// write out the tile information for the screen
		if(!fwrite(tile_data, SCREEN_TILE_HEIGHT * SCREEN_TILE_WIDTH * sizeof(DUMMY_TILE), 1, stream))
			return false;

		// write out more garbage
		if(!fwrite(&dummy_array, 540, 1, stream))
			return false;

		// write out the sprite data
		for (int cur_sprite = 0; cur_sprite < MAX_SPRITES; cur_sprite++)
		{
			// check if there's anything in the sprite block
			if (screen[i]->sprite[cur_sprite] != NULL)
			{
				screen[i]->sprite[cur_sprite]->formatOutput(sprite_data);

				// write out a sprite block
				if (fwrite(&sprite_data, sizeof(sprite_data), 1, stream) == 0)
					return false;
			}
			else
			{
				// write out a sprite block
				if (fwrite(&dummy_array, sizeof(DUMMY_SPRITE), 1, stream) == 0)
					return false;
			}
		}

		if(!fwrite(screen[i]->script, BASE_SCRIPT_MAX_LENGTH, 1, stream))
			return false;
		
		// write out garbage
		if(!fwrite(&dummy_array, 1019, 1, stream))
			return false;
	}

	// close the map.dat file
	fclose(stream);

	return true;
}
