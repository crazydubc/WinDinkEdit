#include "StdAfx.h"

#include <ddraw.h>

#include "Globals.h"
#include "Map.h"
#include "Screen.h"
#include "Sequence.h"
#include "MainFrm.h"
#include "GraphicsManager.h"
#include "LeftView.h"
#include "Engine.h"
#include "Minimap.h"
#include "Colors.h"
#include "Structs.h"
#include "Common.h"
#include "Undo.h"
#include "Tools.h"

Map::Map()
{
}

Map::Map(CString dmod_path)
{
	undo_buffer = new Undo();
	
	dmod_path += "\\";
	this->dmod_path = dmod_path;

	need_resizing = true;
	cur_vision = 0;
	locked_screen = -1;
	screenmatch = false;
	render_state = SHOW_MINIMAP;
	screenMode = SPRITE_MODE;

	hover_sprite = NULL;
	hover_sprite_num = -1;

	screen_x_hover = 0;
	screen_y_hover = 0;
	screen_hover = 0;
	x_hover = 0;
	y_hover = 0;

	mouseSprite = NULL;

	// set the map position to the upper left corner, necessary???
	window.left = 0;
	window.top = 0;

	// fill in the stuct for drawing the blank screens
	memset(&blankscreen, 0, sizeof(DDBLTFX));
	blankscreen.dwSize = sizeof(DDBLTFX);
	blankscreen.dwFillColor = color_blank_screen_fill;

	//zero out the sequence memory
	memset(sequence, 0, MAX_SEQUENCES * sizeof(Sequence*));

	memset(vision_used, 0, VISIONS_TRACKED);

	open_dmod();
}

void Map::finishLoading()
{
	minimap = new Minimap();
	sprite_library.readList();
	sprite_selector.createList();
	mainWnd->GetLeftPane()->PopulateTree();
	Game_Main();
}

Map::~Map()
{
	if (minimap)
	{
		delete minimap;
		minimap = NULL;
	}

	if (undo_buffer)
	{
		delete undo_buffer;
		undo_buffer = NULL;
	}

	// mouse_sprite
	if (mouseSprite != NULL)
	{
		delete mouseSprite;
		mouseSprite = NULL;
	}

	deallocate_map();
	hard_tile_selector.unloadAllBitmaps();
	release_tile_bmps();
	deallocate_sequences();

	Game_Shutdown();
}

// update the coordinates of the view window
void Map::updateMapPosition(int x, int y)
{
	window.left = x;
	window.top = y;

	// verify coordinates are within bounds
	if (window.left < 0)
	{
		window.left = 0;
	}
	if (window.top < 0)
	{
		window.top = 0;
	}

	window.right = window.left + window_width;
	window.bottom = window.top + window_height;

	if (window.right > MAX_MAP_X)
	{
		window.left -=  window.right - MAX_MAP_X;
		window.right = MAX_MAP_X;
	}
	if (window.bottom > MAX_MAP_Y)
	{
		window.top -=  window.bottom - MAX_MAP_Y;
		window.bottom = MAX_MAP_Y;
	}

	screens_displayed.left = window.left / (SCREEN_WIDTH + screen_gap);
	screens_displayed.right = window.right / (SCREEN_WIDTH + screen_gap);
	screens_displayed.top = window.top / (SCREEN_HEIGHT + screen_gap);
	screens_displayed.bottom = window.bottom / (SCREEN_HEIGHT + screen_gap);

	screen_x_offset = -(window.left % (SCREEN_WIDTH + screen_gap));
	screen_y_offset = -(window.top % (SCREEN_HEIGHT + screen_gap));
}

void Map::updateHoverPosition(int x, int y)
{
	screen_x_hover = (x + window.left) / (SCREEN_WIDTH + screen_gap);
	screen_y_hover = (y + window.top) / (SCREEN_HEIGHT + screen_gap);
	screen_hover = screen_y_hover * MAP_COLUMNS + screen_x_hover;

	x_hover = (x + window.left) % (SCREEN_WIDTH + screen_gap);
	y_hover = (y + window.top) % (SCREEN_HEIGHT + screen_gap);

	mainWnd->updateMousePos(screen_hover + 1, x_hover + SIDEBAR_WIDTH, y_hover);

	Sprite* new_hover_sprite = getSpriteClick(hover_sprite_num);

	if (new_hover_sprite != hover_sprite)
	{
		hover_sprite = new_hover_sprite;
		Game_Main();
	}
}

void Map::undoAction()
{
	undo_buffer->undoAction();
}

void Map::redoAction()
{
	undo_buffer->redoAction();
}

void Map::setMouseSprite(Sprite* sprite)
{
	if (sprite)
	{
		if (!mouseSprite)
		{
			mouseSprite = new Sprite();
		}
		memcpy(mouseSprite, sprite, sizeof(Sprite));
	}
	else
	{
		if (mouseSprite)
		{
			delete mouseSprite;
			mouseSprite = NULL;
		}
	}
}

void Map::setVision(int new_vision)
{
	cur_vision = new_vision;
	mainWnd->updateVision(new_vision);
}

// triggered by an undo action
void Map::placeTiles(int first_x_tile, int first_y_tile, int width, int height, 
					 TILEDATA tile_set[SCREEN_TILE_WIDTH * 2][SCREEN_TILE_HEIGHT * 2])
{
	int cur_screen, x_tile, y_tile;
	for (int y = first_y_tile, y_temp = 0; y_temp < height; y++, y_temp++)
	{
		for (int x = first_x_tile, x_temp = 0; x_temp < width; x++, x_temp++)
		{
			cur_screen = MAP_COLUMNS * (y / SCREEN_TILE_HEIGHT) + x / SCREEN_TILE_WIDTH;
			if (current_map->screen[cur_screen] == NULL)
				continue;
			x_tile = x % SCREEN_TILE_WIDTH;
			y_tile = y % SCREEN_TILE_HEIGHT;

			miniupdated[cur_screen] = false;

			screen[cur_screen]->tiles[y_tile][x_tile] = tile_set[x_temp][y_temp];
		}
	}
}

int Map::placeSprite(int x, int y, bool stamp)
{
	bool sprite_placed = false;

	vector<int> *screen_num = new vector<int>;
	vector<int> *sprite_num = new vector<int>;
	vector<Sprite*> *sprite_list = new vector<Sprite*>;

	// use screenmatch when laying down sprites
	if (screenmatch & (locked_screen == -1))
	{
		Sequence* cur_seq = sequence[mouseSprite->sequence];
		int cur_frame = mouseSprite->frame - 1;

		if (cur_seq == NULL || cur_seq->frame_image[cur_frame] == NULL)
			return false;

		//int center_x, center_y;
		//cur_seq->getCenter(cur_frame, center_x, center_y);

		RECT sprite_bounds;
		if (mouseSprite->getImageBounds(sprite_bounds) == false)
			return false;

		sprite_bounds.left += SIDEBAR_WIDTH;
		sprite_bounds.right += SIDEBAR_WIDTH;

		int first_x_screen = (window.left + sprite_bounds.left) / (SCREEN_WIDTH + screen_gap);
		int last_x_screen = (window.left + sprite_bounds.right) / (SCREEN_WIDTH + screen_gap);
		int first_y_screen = (window.top + sprite_bounds.top) / (SCREEN_HEIGHT + screen_gap);
		int last_y_screen = (window.top + sprite_bounds.bottom) / (SCREEN_HEIGHT + screen_gap);

		int cur_screen;
		for (int x_screen = first_x_screen; x_screen <= last_x_screen; x_screen++)
		{
			for (int y_screen = first_y_screen; y_screen <= last_y_screen; y_screen++)
			{
				cur_screen = y_screen * MAP_COLUMNS + x_screen;
				if (screen[cur_screen] != NULL)
				{
					Sprite *new_sprite = new Sprite;
					memcpy(new_sprite, mouseSprite, sizeof(Sprite));

					// add sprite to screen
					new_sprite->x = x + window.left - (x_screen * (SCREEN_WIDTH + screen_gap)) + SIDEBAR_WIDTH;
					new_sprite->y = y + window.top - (y_screen * (SCREEN_HEIGHT + screen_gap));
					compensateScreenGap(new_sprite->x, new_sprite->y);

					screen_num->push_back(cur_screen);
					sprite_num->push_back(screen[cur_screen]->findFirstAvailableSpriteSlot());
					
					sprite_list->push_back(new_sprite);

					sprite_placed = true;
				}
			}
		}
	}
	else 
	{
		int cur_screen;
		// no screenmatch, just place a single sprite
		
		int x_screen, y_screen;

		if (locked_screen == -1)
		{
			x_screen = (x + window.left) / (SCREEN_WIDTH + screen_gap);
			y_screen = (y + window.top) / (SCREEN_HEIGHT + screen_gap);
			cur_screen = y_screen * MAP_COLUMNS + x_screen;
		}
		else
		{
			x_screen = locked_screen % MAP_COLUMNS;
			y_screen = locked_screen / MAP_COLUMNS;
			cur_screen = locked_screen;
		}

		if (screen[cur_screen] == NULL)
			return false;

		Sprite *new_sprite = new Sprite;
		memcpy(new_sprite, mouseSprite, sizeof(Sprite));

		new_sprite->x = x + window.left - (x_screen * (SCREEN_WIDTH + screen_gap)) + SIDEBAR_WIDTH;
		new_sprite->y = y + window.top - (y_screen * (SCREEN_HEIGHT + screen_gap));

		// make sure it returns true before deleting mouseSprite
		compensateScreenGap(new_sprite->x, new_sprite->y);
		sprite_placed = true;

		screen_num->push_back(cur_screen);
		sprite_num->push_back(screen[cur_screen]->findFirstAvailableSpriteSlot());
		sprite_list->push_back(new_sprite);
	}

	if (!sprite_placed)
	{
		delete screen_num;
		delete sprite_num;
		delete sprite_list;

		return false;
	}

	// create the action and then execute to actually place the sprites
	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_SPRITE_PLACE;
	action->sprite_place.screen_num = screen_num;
	action->sprite_place.sprite_num = sprite_num;
	action->sprite_place.sprite_list = sprite_list;
	action->sprite_place.stamp = stamp;

	current_map->undo_buffer->addUndo(action);

	return true;
}

// centers the mouse on the sprites position
void Map::loadMouseSprite(Sprite* sprite, int screen_num)
{
	// create a new mouse sprite if none exists already
	if (mouseSprite == NULL)
	{
		mouseSprite = new Sprite();
	}
	
	memcpy(mouseSprite, sprite, sizeof(Sprite));

	sprite_selector.currentSequence = mouseSprite->sequence;
	sprite_selector.currentFrame = mouseSprite->frame;
	sprite_selector.showFrames = true;

	int screen_x = screen_num % MAP_COLUMNS;
	int screen_y = screen_num / MAP_COLUMNS;
	
	int x_offset = (screen_x * (SCREEN_WIDTH + screen_gap)) - window.left;
	int y_offset = (screen_y * (SCREEN_HEIGHT + screen_gap)) - window.top;

	mouseSprite->x += x_offset;
	mouseSprite->y += y_offset;

	// center the mouse on the sprite
	SetCursorPos(mouseSprite->x + mapRect.left, mouseSprite->y + mapRect.top);
}

// stores sprite in mousesprite
int Map::pickupSprite()
{
	// make sure the screen is valid
	if (screen[screen_hover] == NULL)
		return NULL;
	
	int sprite_num;
	Sprite* temp_sprite;
	if ((temp_sprite = screen[screen_hover]->getSprite(x_hover, y_hover, sprite_num)) == NULL)
		return false;
	

	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_SPRITE_PICKUP;
	action->sprite_pickup.screen_num = screen_hover;
	action->sprite_pickup.sprite_num = sprite_num;
	action->sprite_pickup.new_mouse_sprite = new Sprite;
	memcpy(action->sprite_pickup.new_mouse_sprite, temp_sprite, sizeof(Sprite));

	hover_sprite = NULL;
	hover_sprite_num = -1;

	current_map->undo_buffer->addUndo(action);

	return true;
}

// returns pointer to sprite that was clicked on
Sprite* Map::getSpriteClick(int &sprite_num)
{
	// make sure the screen is valid
	if (screen[screen_hover] == NULL)
		return NULL;

	return screen[screen_hover]->getSprite(x_hover, y_hover, sprite_num);
}

int Map::drawScreens()
{
	memset(new_vision_used, 0, VISIONS_TRACKED);

	int cur_screen;
	RECT minimap_dest_rect, minimap_src_rect;

	double width_ratio = double(SQUARE_WIDTH) / double(SCREEN_WIDTH);
	double height_ratio = double(SQUARE_HEIGHT) / double(SCREEN_HEIGHT);

	int x_total_offset;
	int y_total_offset = screen_y_offset;

	for (int y_screen = screens_displayed.top; y_screen <= screens_displayed.bottom; y_screen++)
	{
		cur_screen = y_screen * MAP_COLUMNS + screens_displayed.left;

		minimap_dest_rect.top = y_screen * SQUARE_HEIGHT;
		minimap_dest_rect.bottom = minimap_dest_rect.top + SQUARE_HEIGHT;

		minimap_src_rect.top = y_total_offset;
		minimap_src_rect.bottom = y_total_offset + SCREEN_HEIGHT;

		if (minimap_src_rect.top < 0)
		{
			minimap_dest_rect.top -= int(double(minimap_src_rect.top) * height_ratio);
			minimap_src_rect.top = 0;
		}

		if (minimap_src_rect.bottom > window_height)
		{
			minimap_dest_rect.bottom -= int(double(minimap_src_rect.bottom - window_height) * height_ratio);
			minimap_src_rect.bottom = window_height;
		}

		if (minimap_src_rect.top >= minimap_src_rect.bottom)
		{
			y_total_offset += (SCREEN_HEIGHT + screen_gap);
			continue;
		}

		x_total_offset = screen_x_offset;
		for (int x_screen = screens_displayed.left; x_screen <= screens_displayed.right; x_screen++)
		{
			minimap_dest_rect.left = x_screen * SQUARE_WIDTH;
			minimap_dest_rect.right = minimap_dest_rect.left + SQUARE_WIDTH;
			
			minimap_src_rect.left = x_total_offset;
			minimap_src_rect.right = x_total_offset + SCREEN_WIDTH;

			if (minimap_src_rect.left < 0)
			{
				minimap_dest_rect.left -= int(double(minimap_src_rect.left) * width_ratio);
				minimap_src_rect.left = 0;
			}

			if (minimap_src_rect.right > window_width)
			{
				minimap_dest_rect.right -= int(double(minimap_src_rect.right - window_width) * width_ratio);
				minimap_src_rect.right = window_width;
			}

			if (minimap_src_rect.left >= minimap_src_rect.right)
			{
				cur_screen++;
				x_total_offset += (SCREEN_WIDTH + screen_gap);
				continue;
			}

			if (screen[cur_screen] != NULL)
			{
				// render tiles
				screen[cur_screen]->drawTiles(x_total_offset, y_total_offset, lpddsback);

				// render sprites
				screen[cur_screen]->drawSprites(x_total_offset, y_total_offset, minimap_src_rect, lpddsback);

				// draw hardness
				if (displayhardness)
				{
					screen[cur_screen]->drawHardTiles(x_total_offset, y_total_offset);
					screen[cur_screen]->drawHardSprites(x_total_offset, y_total_offset, minimap_src_rect);
				}

				// add screen to minimap
				if (update_minimap)
				{   
					drawScreen(cur_screen);
					//minimap->image->Blt(&minimap_dest_rect, lpddsback, &minimap_src_rect, DDBLT_WAIT, NULL);
				}

				// draw a box around the screen if necessary
				if (screenMode == SPRITE_MODE)
				{
					// "lock" the screen
					if (locked_screen == cur_screen)
					{
						RECT dest;
						dest.left = x_total_offset;
						dest.top = y_total_offset;
						dest.right = x_total_offset + SCREEN_WIDTH;
						dest.bottom = y_total_offset + SCREEN_HEIGHT;
						
						if (locked_screen % MAP_COLUMNS != 0)
							dest.left -= screen_gap;
						if (locked_screen >= MAP_COLUMNS)
							dest.top -= screen_gap;
						if (locked_screen % MAP_COLUMNS != MAP_COLUMNS - 1)
							dest.right += screen_gap;
						if (locked_screen < NUM_SCREENS - MAP_COLUMNS)
							dest.bottom += screen_gap;

						drawBox(dest, color_screen_selected_box, 1);
					}
				}
			}
			else
			{
				// draw a blank screen on the main screen
				lpddsback->Blt(&minimap_src_rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blankscreen);

				// draw a blan screen on the minimap
		//		minimap->image->Blt(&minimap_dest_rect, lpddsback, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blankscreen);
			}

			cur_screen++;
			x_total_offset += (SCREEN_WIDTH + screen_gap);
		}
		y_total_offset += (SCREEN_HEIGHT + screen_gap);
	}

	hover_sprite = getSpriteClick(hover_sprite_num);

	
	RECT bounds;
	int x_offset, y_offset;

	// draw a box around the sprite the mouse is on top of
	if (screenMode == SPRITE_MODE && hover_sprite != NULL && mouseSprite == NULL)
	{
		hover_sprite->getImageBounds(bounds);

		RECT clip_box = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

		if (fixBounds(bounds, clip_box))
		{
			x_offset = (screen_x_hover - screens_displayed.left) * 
				(SCREEN_WIDTH + screen_gap) + screen_x_offset;
			y_offset = (screen_y_hover - screens_displayed.top) * 
				(SCREEN_HEIGHT + screen_gap) + screen_y_offset;

			bounds.left += x_offset;
			bounds.right += x_offset;
			bounds.top += y_offset;
			bounds.bottom += y_offset;

			drawBox(bounds, color_sprite_hover_box, 1);
		}	

		if (hover_sprite_info)
		{
			if (hover_sprite_num >= 0 && hover_sprite_num < 100)
			{
				current_map->sequence[hover_sprite->sequence]->drawSpriteInfo(x_offset, y_offset-20, hover_sprite, hover_sprite_num+1);
			}
		}

		if (hover_sprite_hardness)
		{
			if (hover_sprite_num >= 0 && hover_sprite_num < 100)
			{
				//RECT clip_box = {0, 0, current_map->window_width, current_map->window_height};
				current_map->sequence[hover_sprite->sequence]->clipRenderHardness(x_offset, y_offset, hover_sprite, bounds);
			}
		}
	}

	// track the first 256 visions used on screen
	if (memcmp(new_vision_used, vision_used, VISIONS_TRACKED) != 0)
	{
		// update the left panel
		memcpy(vision_used, new_vision_used, VISIONS_TRACKED);
	
		mainWnd->GetLeftPane()->UpdateVisionTree();
	}
	
	return true;
}


void Map::trimSprite(RECT trim)
{
	Sprite *new_sprite = new Sprite;
	memcpy(new_sprite, mouseSprite, sizeof(Sprite));

	if (sequence[new_sprite->sequence] == NULL)
		return;

	RECT image_bounds;
	if (mouseSprite->getImageBounds(image_bounds) == false)
		return;

	int max_x = image_bounds.right - image_bounds.left;
	int max_y = image_bounds.bottom - image_bounds.top;

	if (new_sprite->trim_right == 0)
	{
		new_sprite->trim_left = 0;
		new_sprite->trim_top = 0;
		new_sprite->trim_right = max_x;
		new_sprite->trim_bottom = max_y;
	}

	new_sprite->trim_left += (short)trim.left;
	new_sprite->trim_top += (short)trim.top;
	new_sprite->trim_right += (short)trim.right;
	new_sprite->trim_bottom += (short)trim.bottom;

	if (new_sprite->trim_left < 0)
	{
		new_sprite->trim_left = 0;
	}

	if (new_sprite->trim_top < 0)
	{
		new_sprite->trim_top = 0;
	}

	if (new_sprite->trim_right > max_x)
	{
		new_sprite->trim_right = max_x;
	}

	if (new_sprite->trim_bottom > max_y)
	{
		new_sprite->trim_bottom = max_y;
	}

	if (new_sprite->trim_left == 0 && new_sprite->trim_top == 0 
		&& new_sprite->trim_right == max_x && new_sprite->trim_bottom == max_y)
	{
		// remove all trimming attributes from the sprite.
		new_sprite->trim_left = 0;
		new_sprite->trim_top = 0;
		new_sprite->trim_right = 0;
		new_sprite->trim_bottom = 0;
	}

	changeSpriteProperties(-1, -1, new_sprite, mouseSprite);
}

// creates a new blank screen on the map
void Map::newScreen(int screen_num)
{
	if (((screen_num >= 0) & (screen_num < NUM_SCREENS)) && (screen[screen_num] == NULL))
	{
		UNDO_ACTION* action = new UNDO_ACTION;
		action->type = UT_SCREEN_CREATE;
		action->screen_delete.screen_num = screen_num;

		current_map->undo_buffer->addUndo(action);
	}
}

// deletes the screen from the map
void Map::deleteScreen(int screen_num)
{
	if ((screen_num >= 0) & (screen_num < NUM_SCREENS) && (screen[screen_num] != NULL))
	{
		UNDO_ACTION* action = new UNDO_ACTION;
		action->type = UT_SCREEN_DELETE;
		action->screen_delete.screen_num = screen_num;
		action->screen_delete.midi_num = midi_num[screen_num];
		action->screen_delete.indoor = indoor[screen_num];
		action->screen_delete.old_screen = new Screen(screen[screen_num]);

		current_map->undo_buffer->addUndo(action);
	}	
}

// places a copied screen onto the map
void Map::pasteScreen(int screen_num, Screen* source_screen, int midi_num, int indoor)
{
	if (((screen_num >= 0) & (screen_num < NUM_SCREENS)) && (screen[screen_num] == NULL))
	{
		UNDO_ACTION* action = new UNDO_ACTION;
		action->type = UT_SCREEN_PASTE;
		action->screen_paste.screen_num = screen_num;
		action->screen_paste.midi_num = midi_num;
		action->screen_paste.indoor = indoor;
		action->screen_paste.new_screen = new Screen(source_screen);

		current_map->undo_buffer->addUndo(action);
	}
}

void Map::changeScreenProperties(int screen_num, const char* script, int midi_num, int indoor)
{
	if (((screen_num >= 0) & (screen_num < NUM_SCREENS)) && (screen[screen_num] != NULL))
	{
		UNDO_ACTION* action = new UNDO_ACTION;
		action->type = UT_SCREEN_PROPERTIES_CHANGE;
		action->screen_properties_change.screen_num = screen_num;

		// save new information
		strcpy(action->screen_properties_change.new_script, script);
		action->screen_properties_change.new_midi = midi_num;
		action->screen_properties_change.new_indoor = indoor;

		// save old information
		strcpy(action->screen_properties_change.old_script, screen[screen_num]->script);
		action->screen_properties_change.old_midi = this->midi_num[screen_num];
		action->screen_properties_change.old_indoor = this->indoor[screen_num];		

		current_map->undo_buffer->addUndo(action);
	}
}

void Map::changeSpriteProperties(int screen_num, int sprite_num, Sprite* new_sprite, Sprite* old_sprite)
{
	UNDO_ACTION* action = new UNDO_ACTION;
	action->type = UT_SPRITE_PROPERTIES_CHANGE;
	action->sprite_property_change.screen_num = screen_num;
	action->sprite_property_change.sprite_num = sprite_num;
	action->sprite_property_change.new_sprite = new_sprite;	// pointer was passed in
	action->sprite_property_change.old_sprite = new Sprite;
	memcpy(action->sprite_property_change.old_sprite, old_sprite, sizeof(Sprite));

	current_map->undo_buffer->addUndo(action);
}

void Map::screenshot(char* filename)
{
	FILE *output;

	// make sure we can open the file first
	if ((output = fopen(filename, "rb")) == NULL)
	{
		return;
	}
	fclose(output);

	if (render_state == SHOW_MINIMAP)
		printScreen(filename, minimap->image);
	else
		printScreen(filename, lpddsback);

}

void Map::toggleAutoUpdateMinimap()
{
	if (update_minimap)
	{
		update_minimap = false;
		//minimap->drawSquares();
	}
	else
	{
		update_minimap = true;
		fast_minimap_update = true;
	}
}

//function used to draw one screen on the minimap
void Map::drawScreen(int cur_screen)
{
	if (miniupdated[cur_screen] && fast_minimap_update)
		return;

	//rects for where to draw and from where
	RECT src_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}; //set for whole buffer
	RECT dst_rect;

	//calculate where the screen is on the minimap row and col
	int row_num = (cur_screen / MAP_COLUMNS) + 1;
	int col_num = (cur_screen % MAP_COLUMNS) + 1;

	//set the rect to the screen on the minimap
	dst_rect.left = (col_num - 1) * SQUARE_WIDTH;
	dst_rect.right = col_num * SQUARE_WIDTH;
	dst_rect.top = (row_num - 1) * SQUARE_HEIGHT;
	dst_rect.bottom = row_num * SQUARE_HEIGHT;

	//set to fill color of black
	DDBLTFX	grid_color;
	memset(&grid_color, 0, sizeof(DDBLTFX));
	grid_color.dwFillColor = 0;

	//fill buffer with black
	lpddshidden->Blt(&src_rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &grid_color);

	//if the screen is acctually there, draw tiles and sprite
	if (current_map->screen[cur_screen] != NULL)
	{
		
		current_map->screen[cur_screen]->drawTiles(0, 0, lpddshidden);
		current_map->screen[cur_screen]->drawSprites(0, 0, src_rect, lpddshidden);
	}
	miniupdated[cur_screen] = true;

	//blit the buffer to the minimap
	minimap->image->Blt(&dst_rect, lpddshidden, &src_rect, DDBLT_WAIT, NULL);
}