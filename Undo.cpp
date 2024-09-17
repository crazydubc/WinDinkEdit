#include "StdAfx.h"

#include "Undo.h"
#include "Globals.h"
#include "Map.h"
#include "Screen.h"
#include "Minimap.h"
#include "Engine.h"
#include "SpriteSelector.h"
#include "Sprite.h"
#include "tile.h"

#include "MainFrm.h"

#include <deque>
using namespace std;

Undo::Undo()
{
	overflowed_undo_stack = false;
}

Undo::~Undo()
{
	while (undo_list.size() > 0)
	{
		popBack(&undo_list);
	}

	while (redo_list.size() > 0)
	{
		popBack(&redo_list);
	}
}

// permanently removes an action from the undo que due to keep que size reasonable
void Undo::popBack(deque<UNDO_ACTION*>* action_list)
{
	UNDO_ACTION* action = action_list->back();
	action_list->pop_back();
	if (action->type == UT_SCREEN_PASTE)
	{
		delete action->screen_paste.new_screen;
	}
	else if (action->type == UT_HARD_TILE_MULTIPLE_CHANGE)
	{
		delete action->hard_tile_multiple_change.old_hard_tiles;
		delete action->hard_tile_multiple_change.new_hard_tiles;
	}
	else if (action->type == UT_SCREEN_DELETE)
	{
		delete action->screen_delete.old_screen;
	}
	else if (action->type == UT_SPRITE_DELETE)
	{
		delete action->sprite_delete.mouse_sprite;
	}
	else if (action->type == UT_SPRITE_CREATE)
	{
		delete action->sprite_create.new_mouse_sprite;

		if (action->sprite_create.old_mouse_sprite)
			delete action->sprite_create.old_mouse_sprite;
	}
	else if (action->type == UT_SCREEN_IMPORT)
	{
		delete action->screen_import.new_screen;
	}
	else if (action->type == UT_SPRITE_PICKUP)
	{
		delete action->sprite_pickup.new_mouse_sprite;
	}
	else if (action->type == UT_SPRITE_PROPERTIES_CHANGE)
	{
		delete action->sprite_property_change.new_sprite;
		delete action->sprite_property_change.old_sprite;
		current_map->miniupdated[action->sprite_property_change.screen_num] = false;
	}
	else if (action->type == UT_SPRITE_PLACE)
	{
		while (action->sprite_place.sprite_list->size() > 0)
		{
			delete action->sprite_place.sprite_list->back();
			action->sprite_place.sprite_list->pop_back();
		}

		delete action->sprite_place.screen_num;
		delete action->sprite_place.sprite_num;
		delete action->sprite_place.sprite_list;
	}

	delete action;
}

void Undo::doAction(UNDO_ACTION* action)
{
	if (action->type == UT_TILES_CHANGE)
	{
		current_map->screenMode = TILE_MODE;
		
		current_map->placeTiles(action->tiles_change.first_x_tile, 
								action->tiles_change.first_y_tile, 
								action->tiles_change.width, 
								action->tiles_change.height, 
								action->tiles_change.new_tiles);

		int screen_num = action->hard_tile_change.screen_num;

		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Action: Placed tiles on screen");
	}
	else if (action->type == UT_SET_DEFAULT_HARD_TILE)
	{
		int x = action->set_default_hard_tile.x;
		int y = action->set_default_hard_tile.y;
		int bmp = action->set_default_hard_tile.bmp;

		tileBmp[bmp]->tile_hardness[y][x] = action->set_default_hard_tile.new_index;

		mainWnd->setStatusText("Action: Set default hard tile.");
	}
	else if (action->type == UT_HARD_TILE_EDIT)
	{
		int hard_tile_num = action->hard_tile_edit.hard_tile_num;

		memcpy(current_map->hard_tile_selector.hardTile[hard_tile_num],
			action->hard_tile_edit.new_tile_data, 
			sizeof(action->hard_tile_edit.new_tile_data));

		current_map->hard_tile_selector.unloadSurface(hard_tile_num);

		mainWnd->setStatusText("Action: Hard Tile Edited");
	}
	else if (action->type == UT_TRANSFORM_HARD_TILE)
	{
		//load into local varibles the hardness to trasnform into
		//and the hard tile number
		int hard_tile = action->transform_hard_tile.tile_num;
		int hardness = action->transform_hard_tile.hardness;

		//unload surface so that the hardness is acctually updated on the screen
		//not just in memory.
		current_map->hard_tile_selector.unloadSurface(hard_tile);

		int index;
		//count up all 2500 hardness values (50x50)
		for (int i = 0; i < 50; i++)
		{
			for (int j = 0; j < 50; j++)
			{
				index = current_map->hard_tile_selector.hardTile[hard_tile][i][j];
				//transform only if the current pixel is hard.

				if (index)
					current_map->hard_tile_selector.hardTile[hard_tile][i][j] = hardness;
			}
		}

		mainWnd->setStatusText("Action: Transformed Hard Tile");
	}
	else if (action->type == UT_SCREEN_CREATE)
	{
		int screen_num = action->screen_create.screen;

		current_map->screen[screen_num] = new Screen();

		current_map->indoor[screen_num] = 0;
		current_map->midi_num[screen_num] = 0;

		current_map->miniupdated[screen_num] = false;

		// update the screen on the minimap
		current_map->minimap->renderMapSquare(screen_num);

		mainWnd->setStatusText("Action: Created new screen");
	}
	else if (action->type == UT_SCREEN_DELETE)
	{
		int screen_num = action->screen_delete.screen_num;

		// delete the map
		delete current_map->screen[screen_num];
		current_map->screen[screen_num] = NULL;
		current_map->midi_num[screen_num] = NULL;
		current_map->indoor[screen_num] = NULL;
		
		current_map->miniupdated[screen_num] = false;

		// update the screen on the minimap
		current_map->minimap->renderMapSquare(screen_num);

		mainWnd->setStatusText("Action: Deleted screen");
	}
	else if (action->type == UT_SCREEN_PASTE)
	{
		int screen_num = action->screen_paste.screen_num;

		// delete the map
		current_map->screen[screen_num] = new Screen(action->screen_paste.new_screen);
		current_map->indoor[screen_num] = action->screen_paste.indoor;
		current_map->midi_num[screen_num] = action->screen_paste.midi_num;
		
		// update the screen on the minimap
		current_map->minimap->renderMapSquare(screen_num);

		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Action: Placed copied screen");
	}
	else if (action->type == UT_SCREEN_IMPORT)
	{
		int screen_num = action->screen_import.screen_num;

		//delete the screen
		current_map->screen[screen_num] = new Screen(action->screen_import.new_screen);
		current_map->indoor[screen_num] = action->screen_import.indoor;
		current_map->midi_num[screen_num] = action->screen_import.midi_num;
		
		current_map->miniupdated[screen_num] = false;

		//update screen on minimap
		current_map->minimap->renderMapSquare(screen_num);

		mainWnd->setStatusText("Action: Screen imported");
	}
	else if (action->type == UT_SCREEN_PROPERTIES_CHANGE)
	{
		int screen_num = action->screen_properties_change.screen_num;

		current_map->midi_num[screen_num] = action->screen_properties_change.new_midi;
		current_map->indoor[screen_num] = action->screen_properties_change.new_indoor;
		strcpy(current_map->screen[screen_num]->script, 
			action->screen_properties_change.new_script);

		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Action: Changed screen properties");
	}
	else if (action->type == UT_HARD_TILE_CHANGE)
	{
		current_map->screenMode = HARDBOX_MODE;
		
		current_map->screenMode = HARDBOX_MODE;
		
		int first_x_tile = action->hard_tile_change.first_tile_x;
		int first_y_tile = action->hard_tile_change.first_tile_y;
		int width = action->hard_tile_change.width;
		int height = action->hard_tile_change.height;

		for (int m_y = first_y_tile, y = 0; y < height; m_y++, y++)
		{
			for (int m_x = first_x_tile, x = 0; x < width; m_x++, x++)
			{
				int cur_screen = MAP_COLUMNS * (m_y / SCREEN_TILE_HEIGHT) + m_x / SCREEN_TILE_WIDTH;

				if (cur_screen > 768 || cur_screen < 1)
					continue;

				if (current_map->screen[cur_screen] == NULL)
					continue;

				int x_tile = m_x % SCREEN_TILE_WIDTH;
				int y_tile = m_y % SCREEN_TILE_HEIGHT;

				current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness = action->hard_tile_change.new_hard_tiles[x][y];
				current_map->miniupdated[cur_screen] = false;
			}
		}

		mainWnd->setStatusText("Action: Placed hard tiles");
	}
	else if (action->type == UT_SPRITE_CREATE)
	{
		current_map->screenMode = SPRITE_MODE;
		
		// now create a sprite with the sequence
		current_map->setMouseSprite(action->sprite_create.new_mouse_sprite);

		current_map->getMouseSprite()->x = mouse_x_position;
		current_map->getMouseSprite()->y = mouse_y_position;

		mainWnd->setStatusText("Action: Created new sprite");
	}
	else if (action->type == UT_SPRITE_DELETE)
	{
		current_map->screenMode = SPRITE_MODE;
		
		// should always be a mouse sprite but just in case
		current_map->setMouseSprite(NULL);

		mainWnd->setStatusText("Action: Deleted sprite");
	}
	else if (action->type == UT_SPRITE_PICKUP)
	{
		current_map->screenMode = SPRITE_MODE;
		int screen_num = action->sprite_pickup.screen_num;
		
		// remove the sprite from the screen
		current_map->screen[screen_num]->removeSprite(action->sprite_pickup.sprite_num);
		
		// load the sprite into the mouse sprite
		Sprite* pickup_sprite = action->sprite_pickup.new_mouse_sprite;
		pickup_sprite->x -= SIDEBAR_WIDTH;
		current_map->loadMouseSprite(pickup_sprite, screen_num);
		pickup_sprite->x += SIDEBAR_WIDTH;
		
		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Action: Picked sprite up");
	}
	else if (action->type == UT_SPRITE_PLACE)
	{
		current_map->screenMode = SPRITE_MODE;

		for (int i = 0; i < action->sprite_place.sprite_list->size(); i++)
		{
			Sprite* place_sprite = (*action->sprite_place.sprite_list)[i];
			int sprite_num = (*action->sprite_place.sprite_num)[i];
			//place_sprite->x -= SIDEBAR_WIDTH;
			current_map->screen[(*action->sprite_place.screen_num)[i]]->addSprite(sprite_num, place_sprite);
			
			current_map->miniupdated[(*action->sprite_place.screen_num)[i]] = false;
			//place_sprite->x += SIDEBAR_WIDTH;
		}

		if (action->sprite_place.stamp == false)
		{
			current_map->setMouseSprite(NULL);
		}

		mainWnd->setStatusText("Action: Placed a sprite");
	}
	else if (action->type == UT_SPRITE_PROPERTIES_CHANGE)
	{
		current_map->screenMode = SPRITE_MODE;
		
		if (action->sprite_property_change.sprite_num == -1)
		{
			// modified sprite was the mouse sprite
			current_map->setMouseSprite(action->sprite_property_change.new_sprite);
			current_map->miniupdated[action->sprite_property_change.screen_num] = false;
		}
		else
		{	
			int screen_num = action->sprite_property_change.screen_num;
			current_map->screen[screen_num]->removeSprite(action->sprite_property_change.sprite_num);
			current_map->screen[screen_num]->addSprite(action->sprite_property_change.sprite_num, action->sprite_property_change.new_sprite);
			current_map->miniupdated[screen_num] = false;
		}

		mainWnd->setStatusText("Action: Changed sprite properties");
	}
	else if (action->type == UT_HARD_TILE_MULTIPLE_CHANGE)
	{
		current_map->screenMode = HARDBOX_MODE;
		
		int first_x_tile = action->hard_tile_multiple_change.first_tile_x;
		int first_y_tile = action->hard_tile_multiple_change.first_tile_y;
		int width = action->hard_tile_multiple_change.width;
		int height = action->hard_tile_multiple_change.height;

		int *array = action->hard_tile_multiple_change.new_hard_tiles;

		for (int m_y = first_y_tile, y = 0; y < height; m_y++, y++)
		{
			for (int m_x = first_x_tile, x = 0; x < width; m_x++, x++)
			{
				int cur_screen = MAP_COLUMNS * (m_y / SCREEN_TILE_HEIGHT) + m_x / SCREEN_TILE_WIDTH;
				if (current_map->screen[cur_screen] == NULL)
					continue;
				int x_tile = m_x % SCREEN_TILE_WIDTH;
				int y_tile = m_y % SCREEN_TILE_HEIGHT;

				if (array[y * width + x] != 0)
				{
					current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness = array[y * width + x];
					current_map->miniupdated[cur_screen] = false;
				}
			}
		}

		mainWnd->setStatusText("Action: Placed multiple hard tiles");
	}

	Game_Main();
}

void Undo::undoAction()
{
	if (undo_list.size() == 0)
	{
		return;
	}
	
	UNDO_ACTION* action = undo_list.front();

	if (current_map->render_state == SHOW_MINIMAP && 
		(action->type != UT_SCREEN_CREATE && action->type != UT_SCREEN_DELETE && 
		action->type != UT_SCREEN_PASTE && action->type != UT_SCREEN_PROPERTIES_CHANGE && action->type != UT_SCREEN_IMPORT))
		return;

	undo_list.pop_front();
	
	// move the action to the redo list
	redo_list.push_front(action);

	if (undo_list.size() == 0 && overflowed_undo_stack == false)
	{
		mainWnd->GetActiveDocument()->SetModifiedFlag(FALSE);
	}

	// change the vision to the vision that was used during the action
	current_map->setVision(action->vision);

	// update the map position to the spot the action took place
	current_map->updateMapPosition(action->x_coord, action->y_coord);

	if (action->type == UT_TILES_CHANGE)
	{
		current_map->screenMode = TILE_MODE;
		
		current_map->placeTiles(action->tiles_change.first_x_tile, 
								action->tiles_change.first_y_tile, 
								action->tiles_change.width, 
								action->tiles_change.height, 
								action->tiles_change.old_tiles);

		mainWnd->setStatusText("Undo Action: Placed tiles on screen");
	}
	else if (action->type == UT_SET_DEFAULT_HARD_TILE)
	{
		int x = action->set_default_hard_tile.x;
		int y = action->set_default_hard_tile.y;
		int bmp = action->set_default_hard_tile.bmp;

		int index = action->set_default_hard_tile.old_index;

		tileBmp[bmp]->tile_hardness[y][x] = index;

		mainWnd->setStatusText("Undo Action: Set default hard tile.");
	}
	else if (action->type == UT_HARD_TILE_EDIT)
	{
		int hard_tile_num = action->hard_tile_edit.hard_tile_num;

		memcpy(current_map->hard_tile_selector.hardTile[hard_tile_num],
			action->hard_tile_edit.old_tile_data, 
			sizeof(action->hard_tile_edit.old_tile_data));

		current_map->hard_tile_selector.unloadSurface(hard_tile_num);

		mainWnd->setStatusText("Undo Action: Hard Tile Edited");
	}
	else if (action->type == UT_TRANSFORM_HARD_TILE)
	{
		int tile_num = action->transform_hard_tile.tile_num;
		
		//unload surface so that the hardness is acctually updated on the screen
		//not just in memory.
		current_map->hard_tile_selector.unloadSurface(tile_num);

		memcpy(current_map->hard_tile_selector.hardTile[tile_num], action->transform_hard_tile.old_tile_data, 2500);

		mainWnd->setStatusText("Undo Action: Transformed Hard Tile");
	}
	else if (action->type == UT_SCREEN_CREATE)
	{
		int screen_num = action->screen_create.screen;

		// delete the map
		delete current_map->screen[screen_num];
		current_map->screen[screen_num] = NULL;
		
		// update the screen on the minimap
		current_map->minimap->renderMapSquare(screen_num);
		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Undo Action: Created new screen");
	}
	else if (action->type == UT_SCREEN_IMPORT)
	{
		int screen_num = action->screen_import.screen_num;

		//delete the imported screen
		delete current_map->screen[screen_num];
		current_map->screen[screen_num] = NULL;

		//update the screen on the minimap
		current_map->minimap->renderMapSquare(screen_num);

		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Undo Action: Inported screen");
	}
	else if (action->type == UT_SCREEN_DELETE)
	{
		int screen_num = action->screen_delete.screen_num;

		current_map->screen[screen_num] = new Screen(action->screen_delete.old_screen);

		current_map->indoor[screen_num] = action->screen_delete.indoor;
		current_map->midi_num[screen_num] = action->screen_delete.midi_num;

		// update the screen on the minimap
		current_map->minimap->renderMapSquare(screen_num);

		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Undo Action: Deleted screen");
	}
	else if (action->type == UT_SCREEN_PASTE)
	{
		int screen_num = action->screen_paste.screen_num;

		// delete the map
		delete current_map->screen[screen_num];
		current_map->screen[screen_num] = NULL;
		
		// update the screen on the minimap
		current_map->minimap->renderMapSquare(screen_num);

		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Undo Action: Placed copied screen");
	}
	else if (action->type == UT_SCREEN_PROPERTIES_CHANGE)
	{
		int screen_num = action->screen_properties_change.screen_num;

		current_map->midi_num[screen_num] = action->screen_properties_change.old_midi;
		current_map->indoor[screen_num] = action->screen_properties_change.old_indoor;
		strcpy(current_map->screen[screen_num]->script, 
			action->screen_properties_change.old_script);

		current_map->miniupdated[screen_num] = false;

		mainWnd->setStatusText("Undo Action: Changed screen properties");
	}
	else if (action->type == UT_HARD_TILE_CHANGE)
	{
		current_map->screenMode = HARDBOX_MODE;
		
		int first_x_tile = action->hard_tile_change.first_tile_x;
		int first_y_tile = action->hard_tile_change.first_tile_y;
		int width = action->hard_tile_change.width;
		int height = action->hard_tile_change.height;

		for (int m_y = first_y_tile, y = 0; y < height; m_y++, y++)
		{
			for (int m_x = first_x_tile, x = 0; x < width; m_x++, x++)
			{
				int cur_screen = MAP_COLUMNS * (m_y / SCREEN_TILE_HEIGHT) + m_x / SCREEN_TILE_WIDTH;

				if (cur_screen > 768 || cur_screen < 1)
					continue;

				if (current_map->screen[cur_screen] == NULL)
					continue;

				int x_tile = m_x % SCREEN_TILE_WIDTH;
				int y_tile = m_y % SCREEN_TILE_HEIGHT;

				current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness = action->hard_tile_change.old_hard_tiles[x][y];
			}
		}

		mainWnd->setStatusText("Undo Action: Placed hard tiles");
	}
	else if (action->type == UT_HARD_TILE_MULTIPLE_CHANGE)
	{
		current_map->screenMode = HARDBOX_MODE;
		
		int first_x_tile = action->hard_tile_multiple_change.first_tile_x;
		int first_y_tile = action->hard_tile_multiple_change.first_tile_y;
		int width = action->hard_tile_multiple_change.width;
		int height = action->hard_tile_multiple_change.height;

		int *array = action->hard_tile_multiple_change.old_hard_tiles;

		for (int m_y = first_y_tile, y = 0; y < height; m_y++, y++)
		{
			for (int m_x = first_x_tile, x = 0; x < width; m_x++, x++)
			{
				int cur_screen = MAP_COLUMNS * (m_y / SCREEN_TILE_HEIGHT) + m_x / SCREEN_TILE_WIDTH;
				if (current_map->screen[cur_screen] == NULL)
					continue;
				int x_tile = m_x % SCREEN_TILE_WIDTH;
				int y_tile = m_y % SCREEN_TILE_HEIGHT;

				current_map->screen[cur_screen]->tiles[y_tile][x_tile].alt_hardness = array[y * width + x];
			}
		}

		mainWnd->setStatusText("Undo Action: Placed multiple hard tiles");
	}
	else if (action->type == UT_SPRITE_CREATE)
	{
		current_map->screenMode = SPRITE_MODE;
		
		// change the mouse sprite to the new one
		current_map->setMouseSprite(action->sprite_create.old_mouse_sprite);

		current_map->getMouseSprite()->x = mouse_x_position;
		current_map->getMouseSprite()->y = mouse_y_position;

		mainWnd->setStatusText("Undo Action: Created new sprite");
	}
	else if (action->type == UT_SPRITE_DELETE)
	{
		current_map->screenMode = SPRITE_MODE;
		
		// change the mouse sprite to the new one
		current_map->setMouseSprite(action->sprite_delete.mouse_sprite);

		current_map->getMouseSprite()->x = mouse_x_position;
		current_map->getMouseSprite()->y = mouse_y_position;

		mainWnd->setStatusText("Undo Action: Deleted sprite");
	}
	else if (action->type == UT_SPRITE_PICKUP)
	{
		current_map->screenMode = SPRITE_MODE;
		
		// delete the mouseSprite
		current_map->setMouseSprite(NULL);

		Sprite* place_sprite = action->sprite_pickup.new_mouse_sprite;
		int sprite_num = action->sprite_pickup.sprite_num;
		
		// place the mouseSprite back on the screen
		//place_sprite->x -= SIDEBAR_WIDTH;
		current_map->screen[action->sprite_pickup.screen_num]->addSprite(sprite_num, place_sprite);
		//place_sprite->x += SIDEBAR_WIDTH;

		current_map->miniupdated[action->sprite_pickup.screen_num] = false;

		mainWnd->setStatusText("Undo Action: Picked sprite up");
	}
	else if (action->type == UT_SPRITE_PLACE)
	{
		current_map->screenMode = SPRITE_MODE;
		
		for (int i = 0; i < action->sprite_place.sprite_list->size(); i++)
		{
			current_map->screen[(*action->sprite_place.screen_num)[i]]->removeSprite((*action->sprite_place.sprite_num)[i]);
			current_map->miniupdated[(*action->sprite_place.screen_num)[i]] = false;
		}

		if (action->sprite_place.stamp)
			return;
		
		// load the sprite into the mouse sprite
		Sprite* place_sprite = (*action->sprite_place.sprite_list)[0];
		place_sprite->x -= SIDEBAR_WIDTH;
		current_map->loadMouseSprite((*action->sprite_place.sprite_list)[0], 
			(*action->sprite_place.screen_num)[0]);
		place_sprite->x += SIDEBAR_WIDTH;

		mainWnd->setStatusText("Undo Action: Placed a sprite");
	}
	else if (action->type == UT_SPRITE_PROPERTIES_CHANGE)
	{
		current_map->screenMode = SPRITE_MODE;
		if (action->sprite_property_change.sprite_num == -1)
		{
			// modified sprite was the mouse sprite
			current_map->setMouseSprite(action->sprite_property_change.old_sprite);
		}
		else
		{
			int screen_num = action->sprite_property_change.screen_num;
			current_map->screen[screen_num]->removeSprite(action->sprite_property_change.sprite_num);
			current_map->screen[screen_num]->addSprite(action->sprite_property_change.sprite_num, action->sprite_property_change.old_sprite);
			current_map->miniupdated[screen_num] = false;
		}

		mainWnd->setStatusText("Undo Action: Changed sprite properties");
	}

	Game_Main();
}

// redoes the next action
void Undo::redoAction()
{
	if (redo_list.size() == 0)
	{
		return;
	}
	
	UNDO_ACTION* action = redo_list.front();

	if (current_map->render_state == SHOW_MINIMAP && 
		(action->type != UT_SCREEN_CREATE && action->type != UT_SCREEN_DELETE && 
		action->type != UT_SCREEN_PASTE && action->type != UT_SCREEN_IMPORT))
		return;

	redo_list.pop_front();
	
	// more the action to the redo list
	undo_list.push_front(action);

	doAction(action);

	// change the vision to the vision that was used during the action
	current_map->setVision(action->vision);

	// update the map position to the spot the action took place
	current_map->updateMapPosition(action->x_coord, action->y_coord);

	mainWnd->GetActiveDocument()->SetModifiedFlag(TRUE);
}

// adds an action to the undo buffer
void Undo::addUndo(UNDO_ACTION *undo_action)
{
	// empty the redo list
	while (redo_list.size() > 0)
	{
		popBack(&redo_list);
	}

	// pop an action off the que if we're at the max level
	while (undo_list.size() > max_undo_level)
	{
		popBack(&undo_list);
		overflowed_undo_stack = true;
	}

	undo_action->vision = current_map->cur_vision;
	undo_action->x_coord = current_map->window.left;
	undo_action->y_coord = current_map->window.top;

	undo_list.push_front(undo_action);

	doAction(undo_action);

	int size = undo_list.size();
	
	mainWnd->GetActiveDocument()->SetModifiedFlag(TRUE);
}
