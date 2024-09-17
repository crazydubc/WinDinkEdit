#include "StdAfx.h"

#include "Globals.h"
#include "ddutil.h"
#include "Map.h"
#include "Engine.h"
#include "Screen.h"
#include "MainFrm.h"
#include "Colors.h"
#include "Common.h"
#include "sprite.h"
#include "SpriteEditor.h"
#include <fstream.h>

SpriteEditor::SpriteEditor()
{
	memset(&hardness_bounds, 0, sizeof(RECT));
	x1 = NULL;
	x2 = NULL;
	y1 = NULL;
	y2 = NULL;
	depth_x = NULL;
	depth_y = NULL;
	edited = false;
}

void SpriteEditor::DrawSprite()
{
	seq = current_map->mouseSprite->sequence;
	frame = current_map->mouseSprite->frame;
	width = current_map->sequence[seq]->frame_width[frame-1];
	height = current_map->sequence[seq]->frame_height[frame-1];
	x_offset = (current_map->window_width - width) / 2;
	y_offset = (current_map->window_height - height) / 2;

	editor_bounds.top = y_offset;
	editor_bounds.bottom = current_map->window_height - y_offset;
	editor_bounds.left = x_offset;
	editor_bounds.right = current_map->window_width - x_offset;

	lpddsback->Blt(&editor_bounds, current_map->sequence[seq]->frame_image[frame-1], NULL, DDBLT_WAIT, NULL);
}

void SpriteEditor::DrawHardness()
{
	if (!depth_x && !depth_y)
	{
		current_map->sequence[seq]->getCenter(frame-1, 100, depth_x, depth_y);
	}


	if (x1 || x2 || y1 || y2)
	{
		hardness_bounds.left = x1 + depth_x;
		hardness_bounds.right = x2 + depth_x;
		hardness_bounds.top = y1 + depth_y;
		hardness_bounds.bottom = y2 + depth_y;
	}else {

		if (current_map->sequence[seq]->frame_info[frame-1])
		{
			//use frame hardness
			depth_y = current_map->sequence[seq]->frame_info[frame-1]->center_y;
			depth_x = current_map->sequence[seq]->frame_info[frame-1]->center_x;
			x1 = current_map->sequence[seq]->frame_info[frame-1]->left_boundary;
			x2 = current_map->sequence[seq]->frame_info[frame-1]->right_boundary;
			y1 = current_map->sequence[seq]->frame_info[frame-1]->top_boundary;
			y2 = current_map->sequence[seq]->frame_info[frame-1]->bottom_boundary;
		}
		else if (current_map->sequence[seq]->left_boundary || 
			current_map->sequence[seq]->right_boundary || 
			current_map->sequence[seq]->top_boundary ||
			current_map->sequence[seq]->left_boundary)
		{
			//use sequence hardness
			depth_y = current_map->sequence[seq]->center_y;
			depth_x = current_map->sequence[seq]->center_x;
			x1 = current_map->sequence[seq]->left_boundary;
			x2 = current_map->sequence[seq]->right_boundary;
			y1 = current_map->sequence[seq]->top_boundary;
			y2= current_map->sequence[seq]->bottom_boundary;
		}
		else
		{
			//use default hardness
			x1 = (depth_x - (current_map->sequence[seq]->frame_width[frame-1] / 4)) - depth_x;
			y1 = (depth_y - (current_map->sequence[seq]->frame_height[frame-1] / 10)) - depth_y;
			x2 = (depth_x + (current_map->sequence[seq]->frame_width[frame-1] / 4)) - depth_x;
			y2 = (depth_y + (current_map->sequence[seq]->frame_height[frame-1] / 10)) - depth_y;
		}
	}
	int x = depth_x + x_offset;
	int y = depth_y + y_offset;

	hardness_bounds.left += x_offset;
	hardness_bounds.right += x_offset;
	hardness_bounds.top += y_offset;
	hardness_bounds.bottom += y_offset;

	RECT depth_dot = {x-1, y-1, x+2, y+2};

	drawFilledBox(hardness_bounds, RGB(128,128,128), false, lpddsback);
	drawFilledBox(depth_dot, RGB(0, 255,255), false, lpddsback);
}

void SpriteEditor::Save()
{
	if (edited)
	{
		mainWnd->GetActiveDocument()->SetModifiedFlag(TRUE);
		edited = false;
		current_map->sequence[seq]->addFrameInfo(frame-1, depth_x, depth_y, x1, y1, x2, y2);

		if (!optimize_dink_ini)
		{
			char buffer[255];
			wsprintf(buffer, "%s%s", current_map->dmod_path, "dink.ini");
			ofstream fout(buffer, ios::app);
			sprintf(buffer, "\nset_sprite_info %i %i %i %i %i %i %i %i\n", seq, frame, depth_x, depth_y, x1, y1, x2, y2);
				
			fout << buffer;
			fout.close();
		}
	}

	//reset the editor
	memset(&hardness_bounds, 0, sizeof(RECT));
	x1 = NULL;
	x2 = NULL;
	y1 = NULL;
	y2 = NULL;
	depth_x = NULL;
	depth_y = NULL;
	
}

void SpriteEditor::SetHardness(int left, int top, int right, int bottom)
{
	//set it edited, and make sure the bounds aren't backwards
	edited = true;
	if (right < left)
	{
		int temp = left;
		left = right;
		right = temp;
	}

	if (bottom < top)
	{
		int temp = top;
		top = bottom;
		bottom = temp;
	}

	x1 = left - x_offset - depth_x;
	x2 = right - x_offset - depth_x;
	y1 = top - y_offset - depth_y;
	y2 = bottom - y_offset - depth_y;
}

void SpriteEditor::SetDepth(int x, int y)
{
	//set it to edited, and do the math of figuring out the dpeth
	edited = true;

	int temp = depth_x - (x - x_offset);

	x1 += temp;
	x2 += temp;

	temp = depth_y - (y - y_offset);

	y2 += temp;
	y1 += temp;

	depth_x = x - x_offset;
	depth_y = y - y_offset;
}