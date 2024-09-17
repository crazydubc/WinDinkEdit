#ifndef _IMPORT_MAP_H_
#define _IMPORT_MAP_H_

#include "Globals.h"
#include "Tile.h"

// DEFINES ////////////////////////////////////////////////

// STRUCTS ////////////////////////////////////////////////

// GLOBALS ////////////////////////////////////////////////

void removeImportMap();

class ImportMap
{
public:
	ImportMap();
	~ImportMap();
	int loadMap(CString pathname);
	int importScreen(CString pathname, int cur_screen, int new_screen);
	bool render();
	void tileClicked();

	int screen_order[NUM_SCREENS];
	int midi_num[NUM_SCREENS];
	int indoor[NUM_SCREENS];

private:
	int source_tile;
	int dest_tile;
	CString src_dmod_path;
};

#endif