#include "StdAfx.h"

#include <ddraw.h>

#include "Screen.h"
#include "Globals.h"
#include "Tile.h"
#include "Map.h"
#include "TileSelector.h"
#include "Engine.h"
#include "Sequence.h"
#include "HardTileSelector.h"
#include "Minimap.h"
#include "Sprite.h"
#include "Common.h"

// creates a blank screen with just grass tiles
Screen::Screen()
{
	// initialize the sprites
	memset(sprite, 0, MAX_SPRITES * sizeof(int));

	// initialize tiles (bitmaps and hardness)
	memset(tiles, 0, SCREEN_TILE_HEIGHT * SCREEN_TILE_WIDTH * sizeof(TILEDATA));

	// set base script to nothing.
	memset(script, 0, BASE_SCRIPT_MAX_LENGTH);
}

Screen::Screen(Screen* copy_screen)
{
	// copy tiles
	memcpy(tiles, copy_screen->tiles, sizeof(TILEDATA) * SCREEN_TILE_HEIGHT * SCREEN_TILE_WIDTH);

	// initialize the sprites
	memset(sprite, 0, MAX_SPRITES * sizeof(int));

	// now copy over the ones necessary
	int cur_sprite = 0;
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		if (copy_screen->sprite[i])
		{
			sprite[cur_sprite] = new Sprite;
			memcpy(sprite[cur_sprite], copy_screen->sprite[i], sizeof(Sprite));
			cur_sprite++;
		}
	}

	// copy over the base script for the screen
	memcpy(script, copy_screen->script, BASE_SCRIPT_MAX_LENGTH);
}

Screen::~Screen()
{
	// free all the sprites on the screen
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		if (sprite[i])
		{
			delete sprite[i];
			sprite[i] = 0;
		}
	}
}

// draw all the hardness for the tiles on the screen
int Screen::drawHardTiles(int x_offset, int y_offset)
{
	int hard_tile_num;

	for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
		{
			if (tiles[y][x].alt_hardness != 0)
			{
				// use the user specified hardness
				hard_tile_num = tiles[y][x].alt_hardness;
			}
			else
			{
				// use the default hardness for this tile
				hard_tile_num = tileBmp[tiles[y][x].bmp]->tile_hardness[tiles[y][x].y][tiles[y][x].x];
			}

			// render the tile
			current_map->hard_tile_selector.renderHardness(x * TILEWIDTH + x_offset, y * TILEHEIGHT + y_offset, 
				hard_tile_num, true);
		}
	}
	return true;
}

// draw all the sprite hardboxes for the screen
int Screen::drawHardSprites(int x_offset, int y_offset, RECT &clip_rect)
{
	Sequence *cur_seq;
	// read in the sprites to a sprite list so we can sort for depth que
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		// render the sprite
		if (sprite[i] != NULL)
		{
			// if vision doesn't match, go on to the next possible sprite
			int sprite_vision = sprite[i]->getVision();
			if ((sprite_vision != current_map->cur_vision) & (sprite_vision != 0))
				continue;

			cur_seq = current_map->sequence[sprite[i]->sequence];
			if (cur_seq != NULL)
			{
				cur_seq->clipRenderHardness(x_offset, y_offset, sprite[i], clip_rect);
			}
		}
	}

	return true;
}

// draw all the tiles for the screen
int Screen::drawTiles(int x_offset, int y_offset, LPDIRECTDRAWSURFACE7 &surface)
{
	RECT dest_rect;

	dest_rect.top    = y_offset;
	dest_rect.bottom = y_offset + TILEHEIGHT;

	for (int y = 0; y < SCREEN_TILE_HEIGHT; y++)
	{
		dest_rect.left   = x_offset;
		dest_rect.right  = x_offset + TILEWIDTH;

		for (int x = 0; x < SCREEN_TILE_WIDTH; x++)
		{
			// render the tile
			tileBmp[tiles[y][x].bmp]->render(dest_rect, 
				tiles[y][x].x, tiles[y][x].y, surface);

			dest_rect.left   += TILEWIDTH;
			dest_rect.right  += TILEWIDTH;
		}
		dest_rect.top    += TILEHEIGHT;
		dest_rect.bottom += TILEHEIGHT;
	}
	return true;
}

///////////////////////////////////////////////////////////

// draw all the sprites to the screen
int Screen::drawSprites(int x_offset, int y_offset, RECT &clip_rect, LPDIRECTDRAWSURFACE7 &surface)
{
	struct SPRITEQUE
	{
		Sprite* sprite;
		int depth, sprite_num;
	};

	int mouseSprite_x_old, mouseSprite_y_old;

	SPRITEQUE sprite_list[MAX_SPRITES + 1];
	memset(sprite_list, 0, (MAX_SPRITES + 1) * sizeof(int));

	// background sprites
	SPRITEQUE background_sprite_list[MAX_SPRITES + 1];
	memset(background_sprite_list, 0, (MAX_SPRITES + 1) * sizeof(int));

	int sprite_vision;

	// read in the sprites to a sprite list so we can sort for depth que
	int sprite_que_length = 0;
	int background_sprite_que_length = 0;
	int temp_depth;
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		// render the sprite
		if (sprite[i] != NULL)
		{
			sprite_vision = sprite[i]->getVision();

			// used for tracking visions on the sidebar
			if (sprite_vision < VISIONS_TRACKED)
			{
				new_vision_used[sprite_vision] = 1;
			}

			// don't draw the sprite if it's vision doesn't match the current one
			// unless the sprite's vision is 0 (appears on all screens)
			if ((sprite_vision != current_map->cur_vision) & (sprite_vision != 0))
				continue;

			// set sprite depth to it's y coordinate if it has a depth of 0
			if (sprite[i]->depth == 0)
				temp_depth = sprite[i]->y;
			else
				temp_depth = sprite[i]->depth;

			if (sprite[i]->type == 0)
			{
				// background sprite
				background_sprite_list[background_sprite_que_length].depth = temp_depth;
				background_sprite_list[background_sprite_que_length].sprite = sprite[i];
				background_sprite_list[background_sprite_que_length].sprite_num = i;
				background_sprite_que_length++;
			}
			else
			{
				// foreground sprite
				sprite_list[sprite_que_length].depth = temp_depth;
				sprite_list[sprite_que_length].sprite = sprite[i];
				sprite_list[sprite_que_length].sprite_num = i;
				sprite_que_length++;
			}
		}
	}

	Sprite* mouseSprite = current_map->getMouseSprite();
	if (mouseSprite != NULL)
	{
		mouseSprite_x_old = mouseSprite->x;
		mouseSprite_y_old = mouseSprite->y;

		mouseSprite->x -= x_offset - SIDEBAR_WIDTH;
		mouseSprite->y -= y_offset;

		// compensate for screen gap
		compensateScreenGap(mouseSprite->x, mouseSprite->y);

		if (mouseSprite->depth == 0)
			temp_depth = mouseSprite->y;
		else
			temp_depth = mouseSprite->depth;

		if (mouseSprite->type == 0)
		{
			// background sprite
			background_sprite_list[background_sprite_que_length].depth = temp_depth;
			background_sprite_list[background_sprite_que_length].sprite = mouseSprite;
			background_sprite_que_length++;
		}
		else
		{
			// foreground sprite
			sprite_list[sprite_que_length].depth = temp_depth;
			sprite_list[sprite_que_length].sprite = mouseSprite;
			sprite_que_length++;
		}
	}

	// now sort the sprites according to their depth que
	// this is a small list, use insertion sort
	int in;
	SPRITEQUE temp;

	// sort background sprites first
	// lowest depth ques first
	for (i = 1; i < background_sprite_que_length; i++)
	{
		temp = background_sprite_list[i];
		for (in = i; in > 0 && background_sprite_list[in-1].depth > temp.depth; in--)
		{
			background_sprite_list[in] = background_sprite_list[in-1];
		}
		background_sprite_list[in] = temp;
	}

	if (sprite_que_length > 0)
	{
		background_sprite_list[background_sprite_que_length] = sprite_list[0];

		// lowest depth ques first
		for (i = 1; i < sprite_que_length; i++)
		{
			temp = sprite_list[i];
			for (in = i + background_sprite_que_length; in > background_sprite_que_length && background_sprite_list[in-1].depth > temp.depth; in--)
			{
				background_sprite_list[in] = background_sprite_list[in-1];
			}
			background_sprite_list[in] = temp;
		}
	}

	sprite_que_length += background_sprite_que_length;


	Sprite* temp_sprite;
	Sequence *cur_seq;
	int sprite_num;

	// now print out the sprites
	for (i = 0; i < sprite_que_length; i++)
	{
		temp_sprite = background_sprite_list[i].sprite;
		sprite_num = background_sprite_list[i].sprite_num;
		cur_seq = current_map->sequence[temp_sprite->sequence];

		if (cur_seq != NULL)
			cur_seq->clipRender(x_offset, y_offset, temp_sprite, clip_rect, sprite_num, show_sprite_info, surface);
	}

	// restore old mousesprite position
	if (mouseSprite != NULL)
	{
		mouseSprite->x = mouseSprite_x_old;
		mouseSprite->y = mouseSprite_y_old;
	}

	return true;
}

// returns the sprite located at the x and y locations and detaches it from the screen if erase=true
Sprite* Screen::getSprite(int x, int y, int &sprite_num)
{
	RECT bounds; 
  
    int old_area = 9999999; //silly rig
    sprite_num = -1; 
  
    for (int i = 0; i < MAX_SPRITES; i++) 
    { 
         // check if sprite exists 
         if (sprite[i] != NULL && (sprite[i]->getVision() == 0 ||  
              sprite[i]->getVision() == current_map->cur_vision)) 
         { 
              if (sprite[i]->getImageBounds(bounds) == TRUE) 
              { 
                   // check bounds 
                   if ((y > bounds.top) & (y < bounds.bottom) & (x > bounds.left) & (x < bounds.right)) 
                   { 
                        int area = (bounds.bottom - bounds.top)*(bounds.right - bounds.left); 
  
                        if (area < old_area) 
                        { 
                             old_area = area; 
                             sprite_num = i; 
                        } 
                   } 
              } 
         } 
	} 
 
	
	if (sprite_num == -1)
		return NULL;

    return sprite[sprite_num]; 
}

// places a sprite on the screen
int Screen::addSprite(int sprite_num, Sprite *new_sprite)
{
	int available_slot = sprite_num;

	for (; available_slot < MAX_SPRITES-1; available_slot++)
	{
		// check if sprite exists
		if (sprite[available_slot] == NULL)
		{
			break;
		}
	}

	if (available_slot == MAX_SPRITES-1)
	{
		MessageBox(NULL, "Dink only allows 100 sprites per screen. Please delete some and try again.", "Error too many sprites", NULL);
		return false;
	}

	// make another mousesprite to add to the next screen
	Sprite* temp = sprite[available_slot] = new Sprite;
	memcpy(temp, new_sprite, sizeof(Sprite));

	return true;
}

// deletes a sprite from the screen
void Screen::removeSprite(int sprite_num)
{
	if (sprite[sprite_num])
	{
		delete sprite[sprite_num];
		sprite[sprite_num] = NULL;
	}
}

// returns the number of the first available sprite slot
int Screen::findFirstAvailableSpriteSlot()
{
	for (int i = 0; i < MAX_SPRITES; i++)
	{
		// check if sprite exists
		if (sprite[i] == NULL)
		{
			return i;
		}
	}

	return -1;
}