#include <StdAfx.h>

#include "Colors.h"
#include "Globals.h"
#include "ddutil.h"
#include "Engine.h"

#define COLOR_TILE_GRID						RGB(255, 200, 0)
#define COLOR_MAP_SCREEN_GRID				RGB(0, 0, 220)
#define COLOR_MOUSE_BOX						RGB(255, 255, 255)
#define COLOR_BLANK_SCREEN_FILL				RGB(0, 0, 0)
#define COLOR_TILE_GRID_IN_BOUNDARY			RGB(0, 255, 0)
#define COLOR_TILE_GRID_OUT_BOUNDARY		RGB(0, 0, 255)
#define COLOR_MINIMAP_SCREEN_SELECTED		RGB(255, 0, 0)
#define COLOR_SCREEN_SELECTED_BOX			RGB(0, 255, 0)
#define COLOR_SPRITE_INFO_TEXT				RGB(255, 0, 0)
#define COLOR_SPRITE_HOVER_BOX				RGB(0, 255, 0)

#define COLOR_SPRITE_HARDNESS				RGB(170, 170, 170)
#define COLOR_WARP_SPRITE_HARDNESS			RGB(255, 0, 0)

#define COLOR_NORMAL_TILE_HARDNESS			RGB(220, 220, 170)
#define COLOR_LOW_TILE_HARDNESS				RGB(0, 220, 220)
#define COLOR_OTHER_TILE_HARDNESS			RGB(255, 0, 255)

#define COLOR_POLYGON_FILL_HARDNESS			RGB(255, 0, 0)

UINT color_tile_grid;
UINT color_mouse_box;
UINT color_blank_screen_fill;
UINT color_tile_grid_in_boundary;
UINT color_tile_grid_out_boundary;
UINT color_map_screen_grid;
UINT color_minimap_screen_selected;
UINT color_screen_selected_box;
UINT color_sprite_info_text;
UINT color_sprite_hover_box;

UINT color_sprite_hardness;
UINT color_warp_sprite_hardness;
UINT color_normal_tile_hardness;
UINT color_low_tile_hardness;
UINT color_other_tile_hardness;

UINT color_polygon_fill_hardness;

int colorformat;
int pixelformat;

int setColors()
{
	// find the correct pixel format
	DDPIXELFORMAT pf;
	memset(&pf, 0, sizeof(DDPIXELFORMAT));
	pf.dwSize = sizeof(DDPIXELFORMAT);
	lpddsprimary->GetPixelFormat(&pf);

	color_tile_grid					= DDColorMatch(lpddsprimary, COLOR_TILE_GRID);
	color_mouse_box					= DDColorMatch(lpddsprimary, COLOR_MOUSE_BOX);
	color_blank_screen_fill			= DDColorMatch(lpddsprimary, COLOR_BLANK_SCREEN_FILL);
	color_tile_grid_in_boundary		= DDColorMatch(lpddsprimary, COLOR_TILE_GRID_IN_BOUNDARY);
	color_tile_grid_out_boundary	= DDColorMatch(lpddsprimary, COLOR_TILE_GRID_OUT_BOUNDARY);
	color_map_screen_grid			= DDColorMatch(lpddsprimary, COLOR_MAP_SCREEN_GRID);
	color_minimap_screen_selected	= DDColorMatch(lpddsprimary, COLOR_MINIMAP_SCREEN_SELECTED);
	color_screen_selected_box		= DDColorMatch(lpddsprimary, COLOR_SCREEN_SELECTED_BOX);
	color_sprite_info_text			= DDColorMatch(lpddsprimary, COLOR_SPRITE_INFO_TEXT);
	color_sprite_hover_box			= DDColorMatch(lpddsprimary, COLOR_SPRITE_HOVER_BOX);

	color_sprite_hardness			= DDColorMatch(lpddsprimary, COLOR_SPRITE_HARDNESS);
	color_warp_sprite_hardness		= DDColorMatch(lpddsprimary, COLOR_WARP_SPRITE_HARDNESS);
	color_normal_tile_hardness		= DDColorMatch(lpddsprimary, COLOR_NORMAL_TILE_HARDNESS);
	color_low_tile_hardness			= DDColorMatch(lpddsprimary, COLOR_LOW_TILE_HARDNESS);
	color_other_tile_hardness		= DDColorMatch(lpddsprimary, COLOR_OTHER_TILE_HARDNESS);

	color_polygon_fill_hardness		= DDColorMatch(lpddsprimary, COLOR_POLYGON_FILL_HARDNESS);


	if (pf.dwRGBBitCount == 16)	// 
	{
		colorformat = 2;
		if (pf.dwGBitMask == 0x07e0)	// 5 - 6 - 5 pixel format
		{
			pixelformat = 565;
		}
		else if (pf.dwGBitMask == 0x03e0)	// 5 - 5 - 5 pixel format
		{
			pixelformat = 555;
		}
	}
	else if (pf.dwRGBBitCount == 24)
	{
		colorformat = 3;
	}
	else if (pf.dwRGBBitCount == 32)
	{
		colorformat = 4;
	}
	else
	{
		colorformat = 5;

		MessageBox(NULL, "Invalid Color Depth, please use either 16, 24, or 32 bit color depth.", "Error: Invalid Color Depth", MB_OK);

		// incompatible color mode, quit
		Game_Shutdown();
		exit(1);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Name: DDColorMatch()
// Desc: Convert a RGB color to a pysical color.
//       We do this by letting GDI SetPixel() do the color matching
//       then we lock the memory and see what it got mapped to.
//-----------------------------------------------------------------------------
UINT DDColorMatch(IDirectDrawSurface7 * pdds, COLORREF rgb)
{
    COLORREF                rgbT;
    HDC                     hdc;
    DWORD                   dw = CLR_INVALID;
    DDSURFACEDESC2          ddsd;
    HRESULT                 hres;

    //
    //  Use GDI SetPixel to color match for us
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
        SetPixel(hdc, 0, 0, rgb);       // Set our value
        pdds->ReleaseDC(hdc);
    }
    //
    // Now lock the surface so we can read back the converted color
    //
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;
    if (hres == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface;                 // Get DWORD
        if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
            dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;  // Mask it to bpp
        pdds->Unlock(NULL);
    }
    //
    //  Now put the color that was there back.
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }
    return dw;
}