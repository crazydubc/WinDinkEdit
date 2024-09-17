#ifndef _MINIMAP_H_
#define _MINIMAP_H_

#define MINI_MAP_WIDTH		SQUARE_WIDTH * MAP_COLUMNS
#define MINI_MAP_HEIGHT		SQUARE_HEIGHT * MAP_ROWS


#define SQUARE_EMPTY_NAME	"S06.bmp"
#define SQUARE_EMPTY		0
#define SQUARE_USED_NAME	"S07.bmp"
#define SQUARE_USED			1
#define SQUARE_MIDI_NAME	"S12.bmp"
#define SQUARE_MIDI			2
#define SQUARE_INDOOR_NAME	"S13.bmp"
#define SQUARE_INDOOR		3

class Minimap
{
public:
	Minimap();
	~Minimap();

	int drawSquares();
	int drawMap();
	int renderMapSquare(int cur_screen);
	int renderImportMap(int screen_order[], int midi_num[], int indoor[]);
	bool render();
	int drawGrid();

	int loadSquare(char *filename, int square_num);
	void loadScreen();
	int restoreSurfaces();

	int loadSurface();
	int unloadSurface();
	int reloadSurface();
	void updateHoverPosition(int x, int y);
	int getScreen() {return hover_screen;}
	
	void resizeScreen();

	LPDIRECTDRAWSURFACE7 image;
	int hover_screen;

private:
	LPDIRECTDRAWSURFACE7 mapSquareImage[4];
	DDBLTFX	grid_color;
	int hover_x_screen, hover_y_screen;
	double x_scale, y_scale;
};

#endif