#ifndef __SPRITE_EDITOR__
#define __SPRITE_EDITOR__
#pragma once

#include "Sprite.h"

class Sequence;

class SpriteEditor
{
public:
	SpriteEditor();
	~SpriteEditor();
	void DrawSprite();
	void DrawHardness();
	void Save();
	void SetHardness(int left, int top, int right, int bottom);
	void SetDepth(int x, int y);

	RECT editor_bounds; //for the bounds to draw sprite
	RECT hardness_bounds; //real sprite bounds
	int y_offset, x_offset; //for recalculating the hardness
	int seq, frame; //our trusty sequence and frame in the ini
	int width, height;
	int x1, x2, y1, y2;
	int depth_x, depth_y;
	bool edited;
};

#endif