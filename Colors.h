#ifndef _COLORS_H_
#define _COLORS_H_

#include <ddraw.h>

// PROTOTYPES /////////////////////////////////////////////

int setColors();
UINT DDColorMatch(IDirectDrawSurface7 * pdds, COLORREF rgb);

// COLORS

extern UINT color_tile_grid;
extern UINT color_mouse_box;
extern UINT color_blank_screen_fill;
extern UINT color_tile_grid_in_boundary;
extern UINT color_tile_grid_out_boundary;
extern UINT color_map_screen_grid;
extern UINT color_minimap_screen_selected;
extern UINT color_screen_selected_box;
extern UINT color_sprite_info_text;
extern UINT color_sprite_hover_box;

extern UINT color_sprite_hardness;
extern UINT color_warp_sprite_hardness;
extern UINT color_normal_tile_hardness;
extern UINT color_low_tile_hardness;
extern UINT color_other_tile_hardness;

extern UINT color_polygon_fill_hardness;

extern int colorformat;
extern int pixelformat;

#endif

