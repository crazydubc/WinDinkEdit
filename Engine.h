#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "WinDinkeditView.h"

#include <ddraw.h>

// PROTOTYPES /////////////////////////////////////////////

int Game_Init();
void Game_Shutdown();
int Game_Main();

int resize_map_window(int new_width, int new_height);
LPDIRECTDRAWCLIPPER DD_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds, RECT clip_list);

// vars

class SpriteSelector;
class TileSelector;

extern DDBLTFX ddbltfx;	// used to fill backbuffer

extern bool draw_box;
extern int mouse_x_position, mouse_y_position;
extern int mouse_x_origin, mouse_y_origin;

#endif