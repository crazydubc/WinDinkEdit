#ifndef _SEQUENCE_H_
#define _SEQUENCE_H_

#include <ddraw.h>

#include "Globals.h"

// DEFINES ////////////////////////////////////////////////

#define UNKNOWN_GRAPHIC		0
#define DMOD_GRAPHIC		1
#define DINK_GRAPHIC		2
#define GRAPHIC_NOT_FOUND	3

#define SPRITE_TYPE_INVISIBLE		2

// STRUCTS ////////////////////////////////////////////////

////////////////////

struct FRAME_INFO
{
	int frame_delay;
	int center_x;
	int center_y;
	int left_boundary;
	int top_boundary;
	int right_boundary;
	int bottom_boundary;
};

struct SET_FRAME_FRAME
{
	int source_seq;
	int source_frame;
	int dest_seq;
	int dest_frame;
};

class Sprite;
struct GRAPHICQUE;

class Sequence
{
public:
	Sequence(int sequence_num, char* graphics_path, int frame_delay, int type, int center_x, int center_y, 
				   int left_boundary, int top_boundary, int right_boundary, int bottom_boundary, bool now);
	~Sequence();

	bool addFrameInfo(int frame_num, int center_x, int center_y, int left, int top, int right, int bottom);
	FILE* extractBitmap(char* bitmap_file);
	bool loadFrame(int cur_frame);
	bool loadFrame(int cur_frame, char* bmp_file);

	bool getBounds(int frame, int size, RECT &rect);
	bool getCenter(int frame, int size, int& center_x, int& center_y);

	int render(int x, int y, int cur_frame);
	int clipRender(int x_offset, int y_offset, Sprite* cur_sprite, RECT &clip_rect, int sprite_num, bool draw_info, LPDIRECTDRAWSURFACE7 &surface);
	int drawSpriteInfo(int x_offset, int y_offset, Sprite* cur_sprite, int sprite_num);
	int clipRenderHardness(int x_offset, int y_offset, Sprite* cur_sprite, RECT &clip_rect);
	
	// for cases where the frame info is different
	FRAME_INFO* frame_info[MAX_FRAMES];

	int set_frame_delay[MAX_FRAMES];
	int set_frame_special[MAX_FRAMES];
	int set_frame_frame_seq[MAX_FRAMES];
	int set_frame_frame_frame[MAX_FRAMES];
	
	char graphics_path[MAX_PATH];

	// stuff that can be different for every frame
	short frame_delay;
	int center_x;
	int center_y;
	int left_boundary;
	int top_boundary;
	int right_boundary;
	int bottom_boundary;
	int size;
	int type;
	int frame_width[MAX_FRAMES];
	int frame_height[MAX_FRAMES];
	bool now;

	// same for every frame
	LPDIRECTDRAWSURFACE7 frame_image[MAX_FRAMES];

private:

	int sequence_num;
	GRAPHICQUE* graphic_que_location[MAX_FRAMES];	// so we don't run out of memory
	DDCOLORKEY color_key; // used to set color key

	int graphic_location;
};


#endif
