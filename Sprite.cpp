#include "StdAfx.h"

#include "Sprite.h"
#include "Sequence.h"
#include "Globals.h"
#include "Map.h"

Sprite::Sprite()
{
	memset(this, 0, sizeof(Sprite));
}

// creates a sprite from a sprite structure used to read the file from the map file
Sprite::Sprite(DUMMY_SPRITE &sprite_data)
{
	x					= sprite_data.x;
	y					= sprite_data.y;
	sequence			= sprite_data.sequence;
	frame				= sprite_data.frame;
	type				= sprite_data.type;
	size				= sprite_data.size;
	brain				= sprite_data.brain;
	speed				= sprite_data.speed;
	base_walk			= sprite_data.base_walk;
	base_idle			= sprite_data.base_idle;
	base_attack			= sprite_data.base_attack;
	timing				= sprite_data.timing;
	depth				= sprite_data.depth;
	hardness			= sprite_data.hardness;
	trim_left			= sprite_data.trim_left;
	trim_top			= sprite_data.trim_top;
	trim_right			= sprite_data.trim_right;
	trim_bottom			= sprite_data.trim_bottom;
	warp_enabled		= sprite_data.warp_enabled;
	warp_screen			= sprite_data.warp_screen;
	warp_x				= sprite_data.warp_x;
	warp_y				= sprite_data.warp_y;
	touch_sequence		= sprite_data.touch_sequence;
	base_death			= sprite_data.base_death;
	hitpoints			= sprite_data.hitpoints;
	defense				= sprite_data.defense;
	sound				= sprite_data.sound;
	vision				= sprite_data.vision;
	nohit				= sprite_data.nohit;
	touch_damage		= sprite_data.touch_damage;
	experience			= sprite_data.experience;
	memcpy(script, sprite_data.script, MAX_SCRIPT_NAME_LENGTH);
}

Sprite::~Sprite()
{

}

void Sprite::formatOutput(DUMMY_SPRITE &sprite_data)
{
	memset(&sprite_data, 0 , sizeof(DUMMY_SPRITE));

	// found a sprite, now fill in the dummy_struct
	sprite_data.x					= x;
	sprite_data.y					= y;
	sprite_data.sequence			= sequence;
	sprite_data.frame				= frame;
	sprite_data.type				= type;
	sprite_data.size				= size;
	sprite_data.brain				= brain;
	sprite_data.speed				= speed;
	sprite_data.base_walk			= base_walk;
	sprite_data.base_idle			= base_idle;
	sprite_data.base_attack			= base_attack;
	sprite_data.timing				= timing;
	sprite_data.depth				= depth;
	sprite_data.hardness			= hardness;
	sprite_data.trim_left			= trim_left;
	sprite_data.trim_top			= trim_top;
	sprite_data.trim_right			= trim_right;
	sprite_data.trim_bottom			= trim_bottom;
	sprite_data.warp_enabled		= warp_enabled;
	sprite_data.warp_screen			= warp_screen;
	sprite_data.warp_x				= warp_x;
	sprite_data.warp_y				= warp_y;
	sprite_data.touch_sequence		= touch_sequence;
	sprite_data.base_death			= base_death;
	sprite_data.hitpoints			= hitpoints;
	sprite_data.defense				= defense;
	sprite_data.sound				= sound;
	sprite_data.vision				= vision;
	sprite_data.nohit				= nohit;
	sprite_data.touch_damage		= touch_damage;
	sprite_data.experience			= experience;

	sprite_data.sprite_exist		= 1;	// sprite exists

	memcpy(sprite_data.script, script, MAX_SCRIPT_NAME_LENGTH);
}

BOOL Sprite::getImageBounds(RECT &bounds)
{
	Sequence* cur_seq = current_map->sequence[sequence];
	int cur_frame = frame - 1;

	//int center_x, center_y;

	// make sure the sequence is valid
	if (cur_seq == NULL)
		return FALSE;

	// fill in the destination rect
	if (cur_seq->getBounds(cur_frame, size, bounds) == false)
		return FALSE;

	bounds.left += x - SIDEBAR_WIDTH;
	bounds.right += x - SIDEBAR_WIDTH;
	bounds.top += y;
	bounds.bottom += y;

	return TRUE;
}