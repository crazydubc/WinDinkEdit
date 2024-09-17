// INCLUDES ///////////////////////////////////////////////

#include "StdAfx.h"

#include "WinDinkeditView.h"
#include "MainFrm.h"

#include "Globals.h"
#include "common.h"
#include "Engine.h"
#include "ImportMap.h"
#include "Sequence.h"
#include "Minimap.h"
#include "Screen.h"
#include "Map.h"
#include "SpriteSelector.h"
#include "SpriteEditor.h"
#include "TileSelector.h"
#include "Interface.h"
#include "Structs.h"
#include "Undo.h"

// DEFINES ////////////////////////////////////////////////

#define SCREEN_ARROW_MOVEMENT	10
#define TRIM_SMALL	1
#define TRIM_MEDIUM	5
#define TRIM_BIG	10

// MACROS /////////////////////////////////////////////////

//#define ABS	(var) ((var < 0) ? (var) : (-var))

// TYPES //////////////////////////////////////////////////

// PROTOTYPES /////////////////////////////////////////////

// GLOBALS ////////////////////////////////////////////////

bool keys_pressed[256];

bool screen_move = false;

int map_x_origin, map_y_origin;	// used for moving the map around

void initializeInput()
{
	memset(keys_pressed, 0, sizeof(keys_pressed));
}

void keyPressed(UCHAR key)
{
	if (!current_map)
		return;

	if (key == KEY_FUNCTION_12)
	{
		if (help_text)
			help_text = false;
		else
			help_text = true;
	}

	if (key == KEY_FUNCTION_5 && help_text)
	{
		//toggle color
		switch (help_text_color)
		{
		case COLOR_BLACK:
			{
				help_text_color = COLOR_RED;
			} break;
		case COLOR_RED:
			{
				help_text_color = COLOR_YELLOW;
			} break;
		case COLOR_YELLOW:
			{
				help_text_color = COLOR_GREEN;
			} break;
		case COLOR_GREEN:
			{
				help_text_color = COLOR_BLUE;
			} break;
		case COLOR_BLUE:
			{
				help_text_color = COLOR_WHITE;
			} break;
		case COLOR_WHITE:
			{
				help_text_color = COLOR_BLACK;
			} break;
		default:
			{
				help_text_color = COLOR_YELLOW;
			}break;
		}
		
		Game_Main();
		return;
	}

	if (current_map->render_state == SHOW_TILE_HARDNESS_EDITOR)
	{
		//minus key
		if (key == 0x6D || key == 0xBD)
		{
			if (tile_brush_size > 1)
				tile_brush_size--;
		}

		//plus key
		if (key == 0x6B || key == 0xBB)
		{
			if (tile_brush_size < 25)
				tile_brush_size++;
		}

		switch (key)
		{
		case KEY_ESC:
			{
				current_map->hard_tile_selector.SaveEditor();
			}break;
		}
		return;
	}

	// convert key to lower case if possible
	key = tolower(key);

	mainWnd->setStatusText("");

	// set key to down so we can tell when it is released
	keys_pressed[key] = true;

	if ((current_map->render_state == SHOW_SCREEN) & (!keys_pressed['z']) & (!keys_pressed['x']))
	{
		int x_move = 0;
		int y_move = 0;

		if(keys_pressed[KEY_ARROW_UP])
		{
			y_move -= SCREEN_ARROW_MOVEMENT;
		}

		if(keys_pressed[KEY_ARROW_DOWN])
		{
			y_move += SCREEN_ARROW_MOVEMENT;
		}

		if(keys_pressed[KEY_ARROW_LEFT])
		{
			x_move -= SCREEN_ARROW_MOVEMENT;
		}

		if(keys_pressed[KEY_ARROW_RIGHT])
		{
			x_move += SCREEN_ARROW_MOVEMENT;
		}
		
		if (x_move != 0 || y_move  != 0)
		{
			current_map->updateMapPosition(current_map->window.left + x_move, 
				current_map->window.top + y_move);
			Game_Main();

			return;
		}
	}

	if (current_map->render_state == SHOW_SCREEN_IMPORTER)
	{
		// exit map importer
		if (key == KEY_ESC)
		{
			removeImportMap();
		}

		return;
	}

	// show tile selector
	if (key == KEY_BACKQUOTE)
	{
		current_map->tile_selector.selectScreen(10);
	}
	if ((key >= '0') & (key <= '9'))
	{
		int new_render = int(key) - int('0') - 1;

		if (new_render == -1)	// 0 = 10
			new_render = 9;

		current_map->tile_selector.selectScreen(new_render);
	}

	// toggle screen match
	if (key == 'm')
	{
		if (current_map->screenmatch)
		{
			current_map->screenmatch = false;
		}
		else
		{
			current_map->screenmatch = true;
		}
		mainWnd->updateScreenMatch(current_map->screenmatch);
	}

	// toggle hard sprite mode
	if (key == 'd')
	{
		if (sprite_hard)
		{
			sprite_hard = false;
		}
		else
		{
			sprite_hard = true;
		}
		mainWnd->updateSpriteHard(sprite_hard);
	}

	switch(key)
	{
	// change vision
	case 'v':	mainWnd->GetRightPane()->OnVision(); break;
	// change screenmode
	case 't':		current_map->screenMode = TILE_MODE; current_map->render_state = SHOW_SCREEN; break;	
	case 'r':		current_map->screenMode = SPRITE_MODE; current_map->render_state = SHOW_SCREEN; break;
	case 'h':		current_map->screenMode = HARDBOX_MODE; current_map->render_state = SHOW_SCREEN;
					displayhardness = true; break;
	case 'l':		mainWnd->GetRightPane()->OnImportScreen(); break;
	}

	if (current_map->render_state == SHOW_SCREEN)
	{
		switch(key)
		{
			// show screen selector (ie, minimap)
		case KEY_TAB:
		case KEY_ESC:	current_map->render_state = SHOW_MINIMAP; break;
		case ' ':		displayhardness = true; break;
		case 'i':		show_sprite_info = true; break;
		}

		if (current_map->screenMode == SPRITE_MODE)
		{
			if (current_map->getMouseSprite() != NULL)
			{
				// check if the mouse sprite needs trimming
				int trim_amount = TRIM_MEDIUM;

				if (keys_pressed['q'])
				{
					current_map->render_state = SHOW_SPRITE_HARDNESS_EDITOR;
				}

				if (keys_pressed[KEY_SHIFT])
				{
					trim_amount = TRIM_BIG;
				}
				
				if (keys_pressed[KEY_CTRL])
				{
					trim_amount = TRIM_SMALL;
				}

				RECT trim = {0, 0, 0, 0};

				if (keys_pressed['z'])
				{
					if (keys_pressed[KEY_ARROW_UP])
						trim.top = -trim_amount;
					if (keys_pressed[KEY_ARROW_DOWN])
						trim.top = trim_amount;
					if (keys_pressed[KEY_ARROW_LEFT])
						trim.left = -trim_amount;
					if (keys_pressed[KEY_ARROW_RIGHT])
						trim.left = trim_amount;
				}

				if (keys_pressed['x'])
				{
					if (keys_pressed[KEY_ARROW_UP])
						trim.bottom = -trim_amount;
					if (keys_pressed[KEY_ARROW_DOWN])
						trim.bottom = trim_amount;
					if (keys_pressed[KEY_ARROW_LEFT])
						trim.right = -trim_amount;
					if (keys_pressed[KEY_ARROW_RIGHT])
						trim.right = trim_amount;
				}

				if (trim.left | trim.top | trim.right | trim.bottom)
				{
					current_map->trimSprite(trim);
				}

				// delete sprite
				if (key == KEY_DELETE)
				{
					if (current_map->getMouseSprite())
					{
						UNDO_ACTION* action = new UNDO_ACTION;
						action->type = UT_SPRITE_DELETE;
						action->sprite_delete.mouse_sprite = new Sprite();
						memcpy(action->sprite_delete.mouse_sprite, current_map->getMouseSprite(), sizeof(Sprite));

						current_map->undo_buffer->addUndo(action);
					}
				}

				// stamp sprite
				if (key == 's')
				{
					current_map->placeSprite(mouse_x_position, mouse_y_position, true);

				}

				if (key == KEY_SHIFT)
				{
					// lock the screen
					current_map->lock_screen();
				}
				if (key == KEY_ENTER)
				{
					current_map->placeSprite(mouse_x_position, mouse_y_position, false);
				}
			}	

			// show sprite selector
			if (key == 'e')
			{
				if (current_map->render_state == SHOW_SPRITE_SELECTOR)
				{
					current_map->render_state = SHOW_SCREEN;
				}
				else
				{
					current_map->render_state = SHOW_SPRITE_SELECTOR;
				}
			}
		}
		else if (current_map->screenMode == TILE_MODE)
		{
			switch (key)
			{
			// copy tile(s)
			case 'c':	current_map->tile_selector.storeScreenTiles(); break;
			// stamp tile(s)
			case 's':	current_map->tile_selector.placeTiles(); break;
			}
		}
		else if (current_map->screenMode == HARDBOX_MODE)
		{
			switch (key)
			{
			case 'e':
				{
					current_map->render_state = SHOW_HARD_TILE_SELECTOR; 
				} break;
			case 'c':			
				{
					current_map->hard_tile_selector.storeScreenTiles();
				} break;
			case 'd':
				{
					current_map->hard_tile_selector.SetDefaultHardTile();
				} break;
			case 's':			
				{
					current_map->hard_tile_selector.placeTiles();
				}break;
			case 'q':			
				{
					current_map->hard_tile_selector.LoadEditor();
				}break;
			case KEY_DELETE:	
				{
					current_map->hard_tile_selector.revertTile(); 
				}break;
			default:break;
			}
		}
	} // end SHOW_SCREEN
	else if (current_map->render_state == SHOW_MINIMAP)
	{
		switch (key)
		{
		// show screen
		case KEY_TAB:	current_map->render_state = SHOW_SCREEN; break;
		// render minimap
		case ' ':		current_map->minimap->drawMap(); break;
		}
	}
	else if (current_map->render_state == SHOW_SPRITE_HARDNESS_EDITOR)
	{
		switch (key)
		{
		case KEY_ESC:	spriteeditor->Save(); current_map->screenMode = SPRITE_MODE; current_map->render_state = SHOW_SCREEN; break;
		}
	}
	else if (current_map->render_state == SHOW_SPRITE_SELECTOR)
	{
		// show screen
		if (key == KEY_ESC)
		{
			if (current_map->sprite_selector.showFrames)
			{
				current_map->sprite_selector.showFrames = false;
			}
			else
			{
				current_map->render_state = SHOW_SCREEN;
			}
		}

		switch (key)
		{
		case KEY_PAGEUP:	current_map->sprite_selector.prevPage(); break;
		case KEY_PAGEDOWN:	current_map->sprite_selector.nextPage(); break;
		}
	}
	else if (current_map->render_state == SHOW_TILE_SELECTOR)
	{
		switch (key)
		{
		// show screen
		case KEY_ESC:		current_map->render_state = SHOW_SCREEN; break;
		// copy tiles into buffer
		case 'c':			current_map->tile_selector.storeTiles(); break;
		case KEY_ENTER:		current_map->tile_selector.storeTiles();
							current_map->render_state = SHOW_SCREEN;
							break;
		}	
	}
	else if (current_map->render_state == SHOW_HARD_TILE_SELECTOR)
	{
		switch (key)
		{
		// show screen
		case KEY_ESC:		current_map->render_state = SHOW_SCREEN; break;
		case KEY_PAGEUP:	current_map->hard_tile_selector.prevPage(); break;
		case KEY_PAGEDOWN:	current_map->hard_tile_selector.nextPage(); break;
		}
	}

	Game_Main();
	return;
}

void keyReleased(UCHAR key)
{
	if (!current_map)
		return;

	key = tolower(key);
	keys_pressed[key] = false;

	current_map->unlock_screen();

	if (current_map->screenMode != HARDBOX_MODE)
		displayhardness = false;

	if (key == KEY_SHIFT)
	{
		current_map->hard_tile_selector.tile_box_points = 0;
	}

	show_sprite_info = false;
	screen_move = false;
	Game_Main();

	return;
}

int mouseLeftPressed(int x, int y)
{
	if (!current_map)
		return true;

	SetTimer(mainWnd->GetSafeHwnd(), TIMER_MOUSE_CHECK, 250, NULL);

	mouseMove(x,y);

	if (current_map->render_state == SHOW_SCREEN)
	{
		// prepare the map to be moved
		if (keys_pressed[KEY_CTRL])
		{
			screen_move = true;
			map_x_origin = current_map->window.left;		// set original x location
			map_y_origin = current_map->window.top;		// set original y location
			mouse_x_origin = x;
			mouse_y_origin = y;
		}
		else
		{
			// might be a sprite, check the screens
			if (current_map->screenMode == SPRITE_MODE)
			{
				if (current_map->getMouseSprite() == NULL)
				{
					current_map->pickupSprite();
				}
				else
				{
					// there is a mouse sprite, now place it on the screen if possible
					if (current_map->placeSprite(x, y, false))
					{
						// mouse sprite was placed, update the screen
						Game_Main();
					}
				}
			}
			else if (current_map->screenMode == TILE_MODE)
			{
				// didn't catch a sprite, start making a box and set start location
				draw_box = true;
				mouse_x_origin = x;
				mouse_y_origin = y;

				current_map->tile_selector.updateScreenGrid(x, y);
				Game_Main();
			}
			else if (current_map->screenMode == HARDBOX_MODE)
			{
				if (keys_pressed[KEY_SHIFT])
				{
					current_map->hard_tile_selector.placeTileBoxPoint(x, y);
				} else {
					draw_box = true;
					mouse_x_origin = x;
					mouse_y_origin = y;

					current_map->hard_tile_selector.updateScreenGrid(x,y);
					Game_Main();
				}
			}
		}
	}
	else if (current_map->render_state == SHOW_SPRITE_HARDNESS_EDITOR)
	{
		draw_box = true;
		mouse_x_origin = x;
		mouse_y_origin = y;
	}
	else if (current_map->render_state == SHOW_TILE_HARDNESS_EDITOR)
	{
		draw_box = true;
		current_map->hard_tile_selector.displayEditor(x,y);
	}
	else if (current_map->render_state == SHOW_MINIMAP)
	{
		// go to tile square
		current_map->minimap->loadScreen();	
	}
	else if (current_map->render_state == SHOW_SPRITE_SELECTOR)
	{
		// try to catch a sprite
		current_map->sprite_selector.getSprite(x, y);
	}
	else if (current_map->render_state == SHOW_TILE_SELECTOR)
	{
		draw_box = true;
		mouse_x_origin = x;
		mouse_y_origin = y;

		current_map->tile_selector.updateGrid(x, y);
		Game_Main();
	}
	else if (current_map->render_state == SHOW_HARD_TILE_SELECTOR)
	{
		current_map->hard_tile_selector.getTile(x, y);
		Game_Main();
	}
	else if (current_map->render_state == SHOW_SCREEN_IMPORTER)
	{
		current_map->import_map->tileClicked();
	}

	return true;
}


int mouseLeftReleased(int x, int y)
{
	if (!current_map)
		return true;

	if (draw_box)
	{
		draw_box = false;

		if (current_map->render_state == SHOW_SPRITE_HARDNESS_EDITOR)
		{
			spriteeditor->SetHardness(mouse_x_origin, mouse_y_origin, x, y);
		}

		Game_Main();
	}
	screen_move = false;
	return true;
}

int mouseLeftDoubleClick(int x, int y)
{
	if (!current_map)
		return true;
	// possibly create a screen
	if (current_map->render_state == SHOW_SCREEN)
	{
		// calculate screen mouse is over
		int x_screen = (current_map->window.left + x) / (SCREEN_WIDTH + screen_gap);
		int y_screen = (current_map->window.top + y) / (SCREEN_HEIGHT + screen_gap);
		int cur_screen = y_screen * MAP_COLUMNS + x_screen;

		current_map->newScreen(cur_screen);
	}

	return true;
}

int mouseRightPressed(int x, int y, CWinDinkeditView* view_window)
{
	if (!current_map)
		return true;

	KillTimer(mainWnd->GetSafeHwnd(), TIMER_MOUSE_CHECK);

	mouseMove(x,y);

	if (current_map->render_state == SHOW_SCREEN)
	{
		// adjust properties of the sprite
		if (current_map->screenMode == SPRITE_MODE)
		{
			// there's no mouse sprite, try to find a sprite and display it's properties
			Sprite* mouseSprite = current_map->getMouseSprite();
			if (mouseSprite == NULL)
			{
				int sprite_num;
				Sprite* temp = current_map->getSpriteClick(sprite_num);
				if (temp != NULL)
				{
					view_window->displaySpriteMenu(temp, current_map->screen_hover, sprite_num, x, y);
				}
				else
				{
					view_window->displayScreenMenu(current_map->screen_hover, x, y);
				}
			}
			// there is a mouse sprite, display the properties of that sprite
			else
			{
				view_window->displaySpriteMenu(mouseSprite, current_map->screen_hover, -1, x, y);
			}
		} else
		{
			view_window->displayScreenMenu(current_map->screen_hover, x, y);
		}
	}
	else if (current_map->render_state == SHOW_MINIMAP)
	{
		int screen = current_map->minimap->getScreen();

		view_window->displayScreenMenu(screen, x, y);
	}
	else if (current_map->render_state == SHOW_SPRITE_HARDNESS_EDITOR)
	{
		spriteeditor->SetDepth(x,y);
	}

	return true;
}

int mouseMove(int x, int y)
{
	if (!current_map)
		return true;

	mouse_x_position = x;
	mouse_y_position = y;

	if (current_map->render_state == SHOW_SCREEN)
	{
		current_map->updateHoverPosition(x, y);

		// move the map around
		if (screen_move)
		{
			//if the mouse is out of the map, return.
			if (mouse_x_position < 1 ||
				mouse_x_position >= current_map->window_width ||
				mouse_y_position < 1 ||
				mouse_y_position >= current_map->window_height)
				return false;

			current_map->updateMapPosition(map_x_origin - (x - mouse_x_origin), map_y_origin - (y - mouse_y_origin));
		}

		if (current_map->screenMode == TILE_MODE)
		{
			if (draw_box)
			{
				RECT tile_box = {mouse_x_origin, mouse_y_origin, mouse_x_position, mouse_y_position};
				current_map->tile_selector.updateScreenGrid(tile_box);
			}
		}
		else if (current_map->screenMode == SPRITE_MODE)
		{
			// update the mouse position
			Sprite* mouseSprite = current_map->getMouseSprite();
			if (mouseSprite != NULL)
			{
				mouseSprite->x = x;
				mouseSprite->y = y;	
			}
			else
			{
				// no need to redraw screen
				if (!screen_move)
					return true;
			}
		}
		else if (current_map->screenMode == HARDBOX_MODE)
		{
			if (keys_pressed[KEY_SHIFT])
			{
				//current_map->hard_tile_selector.placeTileBoxMouse(x, y);
			} else if (draw_box) {
				RECT tile_box = {mouse_x_origin, mouse_y_origin, mouse_x_position, mouse_y_position};
				current_map->hard_tile_selector.updateScreenGrid(tile_box);
			}

		}
	}
	else if (current_map->render_state == SHOW_MINIMAP)
	{
		// displays screen coordinates of mouse pointer
		current_map->minimap->updateHoverPosition(x, y);	
	}
	else if (current_map->render_state == SHOW_SPRITE_HARDNESS_EDITOR)
	{
		if (draw_box)
		{
			RECT hard_box;
			hard_box.top = mouse_y_origin;
			hard_box.left = mouse_x_origin;
			hard_box.right = mouse_x_position;
			hard_box.bottom = mouse_y_position;

			spriteeditor->DrawSprite();
			drawFilledBox(hard_box, RGB(128,128,128), true, lpddsback);
		}

	}
	else if (current_map->render_state == SHOW_TILE_HARDNESS_EDITOR)
	{
		//current_map->hard_tile_selector.hardrender(x,y);
	}
	else if (current_map->render_state == SHOW_TILE_SELECTOR)
	{
		if (draw_box)
		{
			current_map->tile_selector.updateGrid(mouse_x_origin, mouse_x_position, mouse_y_origin, mouse_y_position);
		}
	}
	else if (current_map->render_state == SHOW_SCREEN_IMPORTER)
	{
		// displays screen coordinates of mouse pointer
		current_map->minimap->updateHoverPosition(x, y);
	}

	Game_Main();
	return true;
}

// changes the currently used vision
int setVision(int new_vision)
{
	current_map->cur_vision = new_vision;
	mainWnd->updateVision(new_vision);

	Game_Main();

	return true;
}

// opens up the default editor for a script
bool editScript(CString filename)
{
	char full_path[250];
	wsprintf(full_path, "%sstory\\%s.c", current_map->dmod_path, filename);

	HINSTANCE temp = ShellExecute(NULL, "open", full_path , NULL, NULL, SW_SHOWNORMAL);

	if ((int)temp < 32)
		ShellExecute(NULL, "open", "notepad", full_path, NULL, SW_SHOWNORMAL);

	return true;
}