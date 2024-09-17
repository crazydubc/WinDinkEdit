#ifndef _STRUCTS_H_
#define _STRUCTS_H_

struct DUMMY_TILE
{
	int tile;
	int garbage;
	int alt_hardness;
	int garbage2[17];
};

struct TILEDATA
{
	short alt_hardness;
	short bmp;
	short x;
	short y;
};

#endif