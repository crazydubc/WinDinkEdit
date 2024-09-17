#ifndef _SPRITE_SELECTOR_H_
#define _SPRITE_SELECTOR_H_

#include "Globals.h"

class SpriteSelector
{
public:
	SpriteSelector();
	~SpriteSelector();
	int render();
	int drawGrid(int x, int y);
	void getSprite(int x, int y);
	void nextPage();
	void prevPage();
	void createList();
	void resizeScreen();

	bool showFrames;
	int currentFrame, currentSequence;

private:
	int max_pics;
	int selectorScreen;
	int max_sprite;
	int sprites_per_row;
	int sprites_per_column;
	int pics_displayed;
	int spriteSelectorList[MAX_SEQUENCES];
};

#endif