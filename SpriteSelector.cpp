#include "StdAfx.h"

#include "SpriteSelector.h"
#include "Map.h"
#include "Globals.h"
#include "ddutil.h"
#include "Engine.h"
#include "Sequence.h"
#include "MainFrm.h"
#include "Colors.h"
#include "Structs.h"
#include "Common.h"
#include "Undo.h"

SpriteSelector::SpriteSelector()
{
	showFrames = false;
}

SpriteSelector::~SpriteSelector()
{

}

void SpriteSelector::nextPage()
{
	if ((selectorScreen + 1) * max_pics < max_sprite)
		selectorScreen++;
}

void SpriteSelector::prevPage()
{
	if (selectorScreen > 0)
		selectorScreen--;
}

void SpriteSelector::createList()
{
	max_sprite = 0;
	for (int i = 0; i < MAX_SEQUENCES; i++)
	{
		if (current_map->sequence[i] != NULL)
		{
			// add sequence to sprite selector list
			spriteSelectorList[max_sprite] = i;

			max_sprite++;
		}
	}
}

void SpriteSelector::resizeScreen()
{
	sprites_per_row = current_map->window_width / (SPRITE_SELECTOR_BMP_WIDTH + SPRITE_SELECTOR_BMP_GAP);
	sprites_per_column = current_map->window_height / (SPRITE_SELECTOR_BMP_HEIGHT + SPRITE_SELECTOR_BMP_GAP);

	max_pics = sprites_per_row * sprites_per_column;

	selectorScreen = 0;
}

// draw the sequences(first frame only) or frames
int SpriteSelector::render()
{
	if (showFrames)
	{
		// draw all frames of current sequence
		pics_displayed = 0;
		int frame_exist;

		// if no frame, stop drawing
		if (current_map->sequence[currentSequence] == NULL)
			return false;

		int maxframe = max_pics;
		if (maxframe >= MAX_FRAMES)
			maxframe = MAX_FRAMES;

		for (int y = 0; y < sprites_per_column && pics_displayed < maxframe; y++)
		{
			for (int x = 0; x < sprites_per_row && pics_displayed < maxframe; x++)
			{
				// draw the frame
				frame_exist = current_map->sequence[currentSequence]->render(
					x * (SPRITE_SELECTOR_BMP_WIDTH + SPRITE_SELECTOR_BMP_GAP), 
					y * (SPRITE_SELECTOR_BMP_HEIGHT + SPRITE_SELECTOR_BMP_GAP), pics_displayed);

				if (!frame_exist)
					return true;

				pics_displayed++;
			}
		}
	}
	else
	{
		// draw first frame of each sequence
		pics_displayed = selectorScreen * max_pics;
		for (int y = 0; y < sprites_per_column && pics_displayed < max_sprite; y++)
		{
			for (int x = 0; x < sprites_per_row && pics_displayed < max_sprite; x++)
			{
				current_map->sequence[ spriteSelectorList[pics_displayed] ]->render(
					x * (SPRITE_SELECTOR_BMP_WIDTH + SPRITE_SELECTOR_BMP_GAP), 
					y * (SPRITE_SELECTOR_BMP_HEIGHT + SPRITE_SELECTOR_BMP_GAP), 0);
				
				pics_displayed++;
			}
		}
	}

	return true;
}

// universal method, works for both frames and sequences with no changes
int SpriteSelector::drawGrid(int x, int y)
{
	int x_sprite = x / (SPRITE_SELECTOR_BMP_WIDTH + SPRITE_SELECTOR_BMP_GAP);
	int y_sprite = y / (SPRITE_SELECTOR_BMP_HEIGHT + SPRITE_SELECTOR_BMP_GAP);

	if (x_sprite >= sprites_per_row || y_sprite >= sprites_per_column)
		return false;

	int pic_selected = y_sprite * sprites_per_row + x_sprite;

	if (!showFrames)
	{
		pic_selected += selectorScreen * max_pics;
	}

	// make sure it is a valid sequence
	if (pic_selected >= pics_displayed)
		return false;

	RECT dest = {(SPRITE_SELECTOR_BMP_WIDTH + SPRITE_SELECTOR_BMP_GAP) * x_sprite - SPRITE_SELECTOR_BMP_GAP,
			(SPRITE_SELECTOR_BMP_HEIGHT + SPRITE_SELECTOR_BMP_GAP) * y_sprite - SPRITE_SELECTOR_BMP_GAP,
			(SPRITE_SELECTOR_BMP_WIDTH + SPRITE_SELECTOR_BMP_GAP) * x_sprite + SPRITE_SELECTOR_BMP_WIDTH + SPRITE_SELECTOR_BMP_GAP,
			(SPRITE_SELECTOR_BMP_HEIGHT + SPRITE_SELECTOR_BMP_GAP) * y_sprite + SPRITE_SELECTOR_BMP_HEIGHT + SPRITE_SELECTOR_BMP_GAP};
	drawBox(dest, color_tile_grid, SPRITE_SELECTOR_BMP_GAP);


	if (showFrames)
	{
		currentFrame = pic_selected;
	}
	else
	{
		currentSequence = spriteSelectorList[pic_selected];
		currentFrame = 0;
	}

	char buffer[50];
	wsprintf(buffer, "Sequence: %d, Frame: %d", currentSequence, currentFrame + 1);
	mainWnd->setStatusText(buffer);

	return true;
}

void SpriteSelector::getSprite(int x, int y) 
{  
	if (showFrames)  
	{ 
		mainWnd->setStatusText(""); 
		current_map->render_state = SHOW_SCREEN; 
		Sprite* new_mouse_sprite = new Sprite; 
  
		new_mouse_sprite->sequence = currentSequence;	
		new_mouse_sprite->frame = currentFrame + 1;  
		new_mouse_sprite->size = 100;  
		new_mouse_sprite->hardness = !sprite_hard; 
		new_mouse_sprite->type = 1;  
		new_mouse_sprite->base_attack = -1;  
		new_mouse_sprite->base_death = -1; 
		new_mouse_sprite->base_idle = -1;  
		new_mouse_sprite->base_walk = -1;  
		new_mouse_sprite->timing = 33; 
		new_mouse_sprite->setVision(current_map->cur_vision); 
 
		UNDO_ACTION* action = new UNDO_ACTION;  
		action->type = UT_SPRITE_CREATE;  

		if (current_map->getMouseSprite() == NULL)  
		{ 
			action->sprite_create.old_mouse_sprite = NULL; 
		} 
		else 
		{ 
			action->sprite_create.old_mouse_sprite = new Sprite();  
			memcpy(action->sprite_create.old_mouse_sprite, current_map->getMouseSprite(), sizeof(Sprite));  
		}

		action->sprite_create.new_mouse_sprite = new_mouse_sprite; 
		current_map->undo_buffer->addUndo(action); 
	} 
	else 
	{ 
		showFrames = true;  
	} 
}