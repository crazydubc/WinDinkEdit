#ifndef _SPRITE_LIBRARY_H_
#define _SPRITE_LIBRARY_H_

#include "Sprite.h"

#define MAX_SPRITE_NAME_LENGTH 20

struct SPRITENODE
{
	SPRITENODE* next;
	SPRITENODE* prev;
	Sprite sprite;
	char name[MAX_SPRITE_NAME_LENGTH];
};

class SpriteLibrary
{
public:
	SpriteLibrary();
	~SpriteLibrary();

	void addSprite(Sprite* cur_sprite, char *name);
	void getSprite(char *name);
	void getNextInitialize();
	bool getNext(char* text);
	void removeSprite(char *name);
	void storeList();
	void readList();

	bool doesExist(char* name);
	void renameSprite(char* old_name, char* new_name);

private:
	SPRITENODE* head;
	SPRITENODE* tail;
	SPRITENODE* current_sprite;
	int size;
};

#endif