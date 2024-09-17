#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "Globals.h"

// DEFINES ////////////////////////////////////////////////

#define MAX_SCRIPT_NAME_LENGTH	13

// needs to be of size 220
struct DUMMY_SPRITE
{
	int x;
	int y;
	int sequence;
	int frame;
	int type;
	int size;
	bool sprite_exist;
	int rotation;	// doesn't work
	int special;
	int brain;
	char script[MAX_SCRIPT_NAME_LENGTH];
	char hit[MAX_SCRIPT_NAME_LENGTH];
	char die[MAX_SCRIPT_NAME_LENGTH];
	char talk[MAX_SCRIPT_NAME_LENGTH];
	int speed;
	int base_walk;
	int base_idle;
	int base_attack;
	int base_hit;	// might work
	int timing;
	int depth;
	int hardness;
	int trim_left;
	int trim_top;
	int trim_right;
	int trim_bottom;
	int warp_enabled;
	int warp_screen;
	int warp_x;
	int warp_y;
	int touch_sequence;
	int base_death;
	int gold_dropped;	// doesn't work
	int hitpoints;
	int strength;	// doesn't work
	int defense;
	int experience;
	int sound;
	int vision;
	int nohit;
	int touch_damage;

	int garbage[5];	// unknown value
};

// GLOBALS ////////////////////////////////////////////////



// PROTOTYPES /////////////////////////////////////////////

class Sprite
{
public:
	Sprite();
	Sprite(DUMMY_SPRITE &sprite_data);
	~Sprite();

	void formatOutput(DUMMY_SPRITE &sprite_data);
	BOOL getImageBounds(RECT &bounds);

	void setVision(int new_vision) {vision = new_vision;}
	int getVision() {return vision;}

	int x;
	int y;
	int sequence;
	int frame;
	short type;
	short size;
	int special;
	short brain;
	char script[MAX_SCRIPT_NAME_LENGTH];
	short speed;
	short base_walk;
	short base_idle;
	short base_attack;
//	int base_hit;
	short timing;
	int depth;
	short hardness;
	short trim_left;
	short trim_top;
	short trim_right;
	short trim_bottom;
	short warp_enabled;
	short warp_screen;
	short warp_x;
	short warp_y;
	short touch_sequence;
	short base_death;
	short hitpoints;
	short defense;
	short experience;
	short sound;
	short nohit;
	short touch_damage;
private:
	short vision;
};


#endif