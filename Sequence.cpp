// INCLUDES ///////////////////////////////////////////////

#include "StdAfx.h"

#include <string.h>
#include <ddraw.h>
#include <fstream.h>
#include <iostream.h>

#include "Sequence.h"
#include "ddutil.h"
#include "Globals.h"
#include "Map.h"
#include "Engine.h"
#include "GraphicsManager.h"
#include "Colors.h"
#include "Structs.h"
#include "Common.h"

Sequence::Sequence(int sequence_num, char* graphics_path, int frame_delay, int type, int center_x, int center_y, 
				   int left_boundary, int top_boundary, int right_boundary, int bottom_boundary, bool now)
{
	strcpy(this->graphics_path, graphics_path);
	this->frame_delay = frame_delay;
	this->center_x = center_x;
	this->center_y = center_y;
	this->left_boundary = left_boundary;
	this->top_boundary = top_boundary;
	this->right_boundary = right_boundary;
	this->bottom_boundary = bottom_boundary;
	this->sequence_num = sequence_num - 1;
	this->type = type;
	this->now = now;

	// set color key
	int color;
	if (type == BLACK)
	{
		color = RGB(0, 0, 0);
	}
	else
	{
		color = DDColorMatch(lpddsprimary, RGB(255, 255, 255));
	}
	memset(set_frame_delay, 0, sizeof(set_frame_delay));
	memset(set_frame_special, 0, sizeof(set_frame_special));
	memset(set_frame_frame_seq, 0, sizeof(set_frame_frame_seq));
	memset(set_frame_frame_frame, 0, sizeof(set_frame_frame_frame));

	color_key.dwColorSpaceLowValue  = color;
	color_key.dwColorSpaceHighValue = color;

	graphic_location = UNKNOWN_GRAPHIC;

	memset(frame_image, 0, MAX_FRAMES * sizeof(int));
	memset(frame_info, 0, MAX_FRAMES * sizeof(int));
}

Sequence::~Sequence()
{
	// free the memory for all of the sequence's frames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		if (frame_image[i])
		{
			frame_image[i]->Release();
			frame_image[i] = NULL;
		}

		if (frame_info[i])
		{
			delete frame_info[i];
			frame_info[i] = NULL;
		}
	}
}

bool Sequence::addFrameInfo(int frame_num, int center_x, int center_y, int left, int top, int right, int bottom)
{
	if (frame_num < 0 || frame_num >= MAX_FRAMES)
		return false;

	if (frame_info[frame_num] == NULL)
	{
		// create a new frame info
		frame_info[frame_num] = new FRAME_INFO;
	}
	FRAME_INFO* new_frame_info = frame_info[frame_num];
	
	// fill out the info for a special frame
	
	new_frame_info->center_x = center_x;
	new_frame_info->center_y = center_y;
	new_frame_info->left_boundary = left;
	new_frame_info->top_boundary = top;
	new_frame_info->right_boundary = right;
	new_frame_info->bottom_boundary = bottom;

	return true;
}

// returns the location in the .ff file of the bitmap
FILE* Sequence::extractBitmap(char* bitmap_file)
{
	FILE* fastfile;
	char filename[13];
	int num_files;
	int file_begin_location;
	int file_end_location = 0;
	int i;

	char* outFile = bitmap_file;
	char ffFile[100] = "dir.ff";

	// clip the filename off then end and append dir.ff to the path
	int string_length = strlen(bitmap_file);
	for (i = string_length - 1; i >= 0; i--)
	{ 
		if (bitmap_file[i] == '\\')
		{
			outFile = bitmap_file + i + 1;
			strncpy(ffFile, bitmap_file, i);
			ffFile[i] = 0;
			strcat(ffFile, "\\dir.ff");
			break;
		}
	}

	// open the fastfile
	if ((fastfile = fopen(ffFile, "rb")) == NULL)
		return NULL;

	// get the number of .bmp files
	if (fread(&num_files, sizeof(int), 1, fastfile) <= 0)
	{
		fclose(fastfile);
		return NULL;
	}

	for (i = 0; i < num_files; i++)
	{
		// get the file location in the fastfile
		if (fread(&file_begin_location, sizeof(int), 1, fastfile) <= 0)
		{
			fclose(fastfile);
			return NULL;
		}

		// get the filename
		if (fread(filename, sizeof(filename), 1, fastfile) <= 0)
		{
			fclose(fastfile);
			return NULL;
		}

		// check if filename matches (case insensitive)
		if (_stricmp(filename, outFile) == 0)
		{
			break;
		}
	}

	// return if nothing was found
	if (i == num_files)
	{
		fclose(fastfile);
		return NULL;
	}

	// seek to the location of the .bmp file
	fseek(fastfile, file_begin_location, SEEK_SET);

	return fastfile;
}

bool Sequence::getBounds(int frame, int size, RECT &rect)
{
	if (frame < 0 || frame >= MAX_FRAMES)
		return false;

	// make sure the frame is valid and has been loaded
	if (frame_image[frame] == NULL)
	{
		if (loadFrame(frame) == false)
			return false;	
		return false;
	}

	double mysize = (double(size) / 100.00);
	int x_buff, y_buff;

	x_buff = (frame_width[frame] / 2) - ((frame_width[frame] * mysize) / 2);
	y_buff = (frame_height[frame] / 2) - ((frame_height[frame] * mysize) / 2);

	//double width_diff = (size_ratio - 1.0) / 2.0 * double(frame_width[cur_frame]);
	//double height_diff = (size_ratio - 1.0) / 2.0 * double(frame_height[cur_frame]);

	int x, y;
	getCenter(frame, size, x, y);

	//lets try this

	rect.left = long(double(-x) + x_buff);
	rect.top = long(double(-y) + y_buff);
	rect.right = long(double(frame_width[frame] - x) - x_buff);
	rect.bottom = long(double(frame_height[frame] - y) - y_buff);

	return true;
}

bool Sequence::getCenter(int frame, int size, int& x, int& y) 
{
	if (frame < 0 || frame >= MAX_FRAMES)
		return false;

	// make sure the frame is valid and has been loaded  
	if (frame_image[frame] == NULL)  
		return false;  

	if (frame_info[frame] == NULL)  
	{
		if (center_x != 0 && center_y != 0) 
		{ 
			x = center_x;  
			y = center_y;  
		} 

		// check if hardbox default should be used  
		else if (type == NOTANIM)  
		{ 
			x = (frame_width[frame] - frame_width[frame] / 2) + frame_width[frame] / 6;  
			y = (frame_height[frame] - frame_height[frame] / 4) - frame_height[frame] / 30;  
		} 
		else 
		{ 
			// use the first frame's information for computing the center  
			if (frame_image[0] == NULL)  
			{ 
				if (loadFrame(0) == false) 
					return false;  
			} 
 
			// permanently change the centers for the sequence 
			x = center_x = (frame_width[0] - frame_width[0] / 2) + frame_width[0] / 6; 
			y = center_y = (frame_height[0] - frame_height[0] / 4) - frame_height[0] / 30;  
		}
	} 
	else 
	{ 
		x = frame_info[frame]->center_x; 
		y = frame_info[frame]->center_y;  
	} 
	return true; 
}

bool Sequence::loadFrame(int cur_frame, char* bmp_file)
{
	if (cur_frame < 0 || cur_frame >= MAX_FRAMES)
		return false;

	FILE* stream;

	if ((stream = fopen(bmp_file, "rb")) != NULL)
	{
		frame_image[cur_frame] = loadBitmap(stream, frame_width[cur_frame], frame_height[cur_frame]);
		fclose(stream);
	}
	// direct load failed, try extracting the file from the "dir.ff"
	else if ( (stream = extractBitmap(bmp_file)) != NULL)
	{
		frame_image[cur_frame] = loadBitmap(stream, frame_width[cur_frame], frame_height[cur_frame]);
		fclose(stream);
	}

	if (frame_image[cur_frame] == NULL)
		return false;

	// now set the color key for source blitting
	frame_image[cur_frame]->SetColorKey(DDCKEY_SRCBLT, &color_key);

	// add the frame to the graphics manager
	graphic_que_location[cur_frame] = graphics_manager.addGraphic(&frame_image[cur_frame]);

	return true;
}

bool Sequence::loadFrame(int cur_frame)
{
	if (cur_frame < 0 || cur_frame >= MAX_FRAMES)
		return false;

	char bmp_file[MAX_PATH];

	if (graphic_location == GRAPHIC_NOT_FOUND)
	{
		// graphic cannot be loaded
		return false;
	}
	else if (graphic_location == UNKNOWN_GRAPHIC)
	{
		// try the dmod directory first
		wsprintf(bmp_file, "%s%s%.2d.bmp", current_map->dmod_path, graphics_path, 1);
		if (loadFrame(0, bmp_file) == true)
		{
			graphic_location = DMOD_GRAPHIC;
		}
		else
		{
			wsprintf(bmp_file, "%s%s%.2d.bmp", dink_path, graphics_path, 1);
			if (loadFrame(0, bmp_file) == true)
			{
				graphic_location = DINK_GRAPHIC;
			}
			else
			{
				graphic_location = GRAPHIC_NOT_FOUND;
				return false;
			}
		}
	}

	if (graphic_location == DMOD_GRAPHIC)
	{
		wsprintf(bmp_file, "%s%s%.2d.bmp", current_map->dmod_path, graphics_path, cur_frame + 1);
		return loadFrame(cur_frame, bmp_file);
	}
	else if (graphic_location == DINK_GRAPHIC)
	{
		wsprintf(bmp_file, "%s%s%.2d.bmp", dink_path, graphics_path, cur_frame + 1);
		return loadFrame(cur_frame, bmp_file);
	}

	return true;
}

// used for drawing sprites in the sprite selector
int Sequence::render(int x, int y, int cur_frame)
{
	if (cur_frame < 0 || cur_frame >= MAX_FRAMES)
		return false;

	if (frame_image[cur_frame] == NULL)
		if (loadFrame(cur_frame) == false)
			return false;

	RECT dest_rect;   // the destination rectangle                          
	// fill in the destination rect

	dest_rect.left   = x;
	dest_rect.top    = y;
	dest_rect.right  = x + SPRITE_SELECTOR_BMP_WIDTH;
	dest_rect.bottom = y + SPRITE_SELECTOR_BMP_HEIGHT;

	HRESULT hr = lpddsback->Blt(&dest_rect, frame_image[cur_frame], NULL, DDBLT_WAIT, NULL);

	// reload the surface if it was lost (usually if a full screen application is run)
	if (hr == DDERR_SURFACELOST)
	{
		if(frame_image[cur_frame]->IsLost())
		{
			frame_image[cur_frame]->Restore();
			loadFrame(cur_frame);
		}
	}

	// notify the graphics manager that the graphic was just used
	graphics_manager.moveLocation(graphic_que_location[cur_frame]);

	return true;
}

// clips sprites to screens, offsets are screen locations
int Sequence::clipRender(int x_offset, int y_offset, Sprite* cur_sprite, RECT &clip_rect, int sprite_num, bool draw_info, LPDIRECTDRAWSURFACE7 &surface)
{
	int cur_frame = cur_sprite->frame - 1;
	if (cur_frame < 0 || cur_frame >= MAX_FRAMES)
		return false;

	size = cur_sprite->size;
	int x = cur_sprite->x + x_offset;
	int y = cur_sprite->y + y_offset; 

	if (frame_image[cur_frame] == NULL)
	{
		if (loadFrame(cur_frame) == false)
			return false;		
	}

	RECT dest_rect;

	if (getBounds(cur_frame, cur_sprite->size, dest_rect) == false)
		return false;

	dest_rect.left		+= x - SIDEBAR_WIDTH;
	dest_rect.top		+= y;
	dest_rect.right		+= x - SIDEBAR_WIDTH;
	dest_rect.bottom	+= y;

	double size_ratio = double(cur_sprite->size) / 100.0;
	
	RECT source_rect;

	// to trim or not to trim
	if (cur_sprite->trim_right == 0)
	{
		// fill in the source rect
		source_rect.left	= 0;
		source_rect.top		= 0;
		source_rect.right	= frame_width[cur_frame];
		source_rect.bottom	= frame_height[cur_frame];
	}
	else
	{
		// trim the sprite
		dest_rect.left += cur_sprite->trim_left;
		dest_rect.top += cur_sprite->trim_top;
		dest_rect.right -= frame_width[cur_frame] - cur_sprite->trim_right;
		dest_rect.bottom -= frame_height[cur_frame] - cur_sprite->trim_bottom;

		source_rect.left = cur_sprite->trim_left;
		source_rect.top = cur_sprite->trim_top;
		source_rect.right = cur_sprite->trim_right;
		source_rect.bottom = cur_sprite->trim_bottom;	
	}

	// now clip sprites according to what screen they are on

	// clip left side
	if (dest_rect.left < clip_rect.left)
	{
		if (cur_sprite->size == 100)
			source_rect.left += clip_rect.left - dest_rect.left;
		else
			source_rect.left += int(double(clip_rect.left - dest_rect.left) / size_ratio);

		dest_rect.left = clip_rect.left;
	}

	// clip right side
	if (dest_rect.right > clip_rect.right)
	{
		if (cur_sprite->size == 100)
			source_rect.right -= dest_rect.right - clip_rect.right;
		else
			source_rect.right -= int(double(dest_rect.right - clip_rect.right) / size_ratio);

		dest_rect.right = clip_rect.right;
	}

	// return if sprite is not present on the screen
	if (dest_rect.left >= dest_rect.right)
		return false;

	// clip top side
	if (dest_rect.top < clip_rect.top)
	{
		if (cur_sprite->size == 100)
			source_rect.top += clip_rect.top - dest_rect.top;
		else
			source_rect.top += int(double(clip_rect.top - dest_rect.top) / size_ratio);

		dest_rect.top = clip_rect.top;
	}

	// clip top side
	if (dest_rect.bottom > clip_rect.bottom)
	{
		if (cur_sprite->size == 100)
			source_rect.bottom -= dest_rect.bottom - clip_rect.bottom;
		else
			source_rect.bottom -= int(double(dest_rect.bottom - clip_rect.bottom) / size_ratio);

		dest_rect.bottom = clip_rect.bottom;
	}

	// return if sprite is not present on the screen
	if (dest_rect.top >= dest_rect.bottom)
		return false;


	HRESULT hr;

	DDBLTFX ddbltfx;
	memset(&ddbltfx, 0, sizeof(DDBLTFX));
	ddbltfx.dwSize = sizeof(DDBLTFX);
	//ddbltfx.dwDDFX = DDBLTFX_ARITHSTRETCHY;

	// blt to destination surface
	if (cur_sprite->type == SPRITE_TYPE_INVISIBLE)
	{
		// draw sprite interlaced
		int src_height = source_rect.bottom - source_rect.top;
		int dest_height = dest_rect.bottom - dest_rect.top;
		double ratio = 2.0 * double(src_height) / double(dest_height);
		double src_row = 0.0;

		RECT source_rect2, dest_rect2;
		dest_rect2.left = dest_rect.left;
		dest_rect2.right = dest_rect.right;
		source_rect2.left = source_rect.left;
		source_rect2.right = source_rect.right;

		for (int dest_row = 0; dest_row < dest_height; dest_row += 2)
		{
			dest_rect2.top = dest_rect.top + dest_row;
			dest_rect2.bottom = dest_rect2.top + 1;
			source_rect2.top = int(src_row) + source_rect.top;
			source_rect2.bottom = source_rect2.top + 1;
			
			hr = surface->Blt(&dest_rect2, frame_image[cur_frame], &source_rect2, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			src_row += ratio;
		}
	}
	else
	{
		// draw solid sprite
		hr = surface->Blt(&dest_rect, frame_image[cur_frame], &source_rect, DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, &ddbltfx);
	}

	
	// check if the surface was lost
	if (hr == DDERR_SURFACELOST)
	{
		if(frame_image[cur_frame]->IsLost())
		{
			frame_image[cur_frame]->Restore();
			loadFrame(cur_frame);
		}
	}

	// notify the graphics manager that the graphic was just used
	graphics_manager.moveLocation(graphic_que_location[cur_frame]);

	//should I draw it now?
	if (draw_info)
		drawSpriteInfo(x_offset, y_offset, cur_sprite, sprite_num+1);

	return true;
}

int Sequence::drawSpriteInfo(int x_offset, int y_offset, Sprite* cur_sprite, int sprite_num)
{
	int cur_frame = cur_sprite->frame - 1;
	if (cur_frame < 0 || cur_frame >= MAX_FRAMES)
		return false;

	size = cur_sprite->size;
	int x = cur_sprite->x + x_offset;
	int y = cur_sprite->y + y_offset; 

	if (frame_image[cur_frame] == NULL)
	{
		return false;
	}

	int center_x2, center_y2;
	
	// fill in the destination rect
	if (getCenter(cur_frame, size, center_x2, center_y2) == false)
		return false;

	center_x2 = x_offset + cur_sprite->x - center_x2 - SIDEBAR_WIDTH;
	center_y2 = y_offset + cur_sprite->y - center_y2 - SIDEBAR_WIDTH;

	char buffer[50];

	RECT fill_box;
	fill_box.left = center_x2 - 5;
	fill_box.right = center_x2 + 100;
	fill_box.top = center_y2 - 5;
	fill_box.bottom = center_y2 + 50;

	RECT clip_box = {0, 0, current_map->window_width, current_map->window_height};

	//if (fixBounds(fill_box, clip_box) == false) //damn gary!
	//	return false;

	//WC's fix second part of making draw info alittle less sloppy
	//if the text box is out of bounds, kill it.
	if (fill_box.bottom > current_map->window_height ||
		fill_box.right > current_map->window_width ||
		fill_box.top < 0 ||
		fill_box.left < 0)
		return false;

	//there
	drawFilledBox(fill_box, 0, false, lpddsback);

	// draw script name
	wsprintf(buffer, "%s", cur_sprite->script);
	Draw_Text_GDI(buffer, fill_box.left + 5, fill_box.top + 35, color_sprite_info_text, lpddsback);

	// draw script brain	
	wsprintf(buffer, "Brain: %d", cur_sprite->brain);
	Draw_Text_GDI(buffer, fill_box.left + 5, fill_box.top + 18, color_sprite_info_text, lpddsback);

	//draw sprite number
	wsprintf(buffer, "Sprite: %d", sprite_num);
	Draw_Text_GDI(buffer, fill_box.left + 5, fill_box.top + 2, color_sprite_info_text, lpddsback);

	return true;
}


// draws the sprite hardness'
int Sequence::clipRenderHardness(int x_offset, int y_offset, Sprite* cur_sprite, RECT &clip_rect)
{
	size = cur_sprite->size;

	int cur_frame = cur_sprite->frame - 1;
	if (cur_frame < 0 || cur_frame >= MAX_FRAMES)
		return false;


	double mysize = (double(size) / 100.00);

	int my_width = frame_width[cur_frame];
	int my_height = frame_height[cur_frame];

	int x_buff = (my_width / 2) - ((my_width * mysize) / 2);
	int y_buff = (my_height / 2) - ((my_height * mysize) / 2);

	if (cur_frame < 0 || cur_frame >= MAX_FRAMES)
		return false;

	if (frame_image[cur_frame] == NULL)
		return false;

	int x = cur_sprite->x + x_offset;
	int y = cur_sprite->y + y_offset;

	RECT dest_rect;

	// fill in the destination rect
	if (frame_info[cur_frame] != NULL)
	{
		dest_rect.left		= (x + (frame_info[cur_frame]->left_boundary * mysize) - SIDEBAR_WIDTH);
		dest_rect.top		= (y + (frame_info[cur_frame]->top_boundary * mysize));
		dest_rect.right		= (x + (frame_info[cur_frame]->right_boundary * mysize) - SIDEBAR_WIDTH);
		dest_rect.bottom	= (y + (frame_info[cur_frame]->bottom_boundary * mysize));
	}
	else
	{
		// check if hardbox default should be used
		if (((center_x == 0) & (center_y == 0)) | ((type != NOTANIM) & 
			(left_boundary == 0) & (top_boundary == 0) & (right_boundary == 0) & (bottom_boundary == 0)))
		{
			dest_rect.left		= (x - (frame_width[cur_frame] * mysize) / 4 - SIDEBAR_WIDTH);
			dest_rect.top		= (y - ((frame_height[cur_frame] * mysize)) / 10);
			dest_rect.right		= (x + (frame_width[cur_frame]* mysize) / 4 - SIDEBAR_WIDTH);
			dest_rect.bottom	= (y + ((frame_height[cur_frame] * mysize)) / 10);
		}
		else
		{
			dest_rect.left		= (x + (left_boundary * mysize) - SIDEBAR_WIDTH);
			dest_rect.top		= (y + (top_boundary * mysize));
			dest_rect.right		= (x + (right_boundary * mysize) - SIDEBAR_WIDTH);
			dest_rect.bottom	= (y + (bottom_boundary * mysize));
		}
	}

	// now clip sprites according to what screen they are on
	int x_max_right = SCREEN_WIDTH + x_offset;
	int y_max_right = SCREEN_HEIGHT + y_offset;

	if (fixBounds(dest_rect, clip_rect) == false)
		return false;

	// set the hardbox color based on whether it is a warp or not
	if (cur_sprite->warp_enabled)
	{
		// red
		hardFill.dwFillColor = color_warp_sprite_hardness;
	}
	else
	{
		// normal hardness color
		hardFill.dwFillColor = color_sprite_hardness;
	}

	// blt to destination surface
	if (cur_sprite->hardness == 1)
	{
		drawBox(dest_rect, hardFill.dwFillColor, 1);
	}
	else
	{
		lpddsback->Blt(&dest_rect, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &hardFill);
	}
	return true;
}