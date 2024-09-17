// INCLUDES ///////////////////////////////////////////////

#include "StdAfx.h"

#include "Globals.h"
#include "Sequence.h"
#include "SpriteLibrary.h"
#include "Map.h"
#include "LeftView.h"
#include "MainFrm.h"
#include "Undo.h"

// DEFINES ////////////////////////////////////////////////

// MACROS /////////////////////////////////////////////////

// TYPES //////////////////////////////////////////////////

// PROTOTYPES /////////////////////////////////////////////

// GLOBALS ////////////////////////////////////////////////


SpriteLibrary::SpriteLibrary()
{
	head = new SPRITENODE;
	tail = new SPRITENODE;

	head->next = tail;
	tail->prev = head;
	size = 0;
}

SpriteLibrary::~SpriteLibrary()
{
	SPRITENODE* temp = head->next;
	SPRITENODE* temp2;

	while (temp != tail)
	{
		temp2 = temp;
		temp = temp->next;
		delete temp2;
	}

	delete head;
	delete tail;

	head = NULL;
	tail = NULL;
}

void SpriteLibrary::addSprite(Sprite* cur_sprite, char *new_name)
{
	// make name lowercase first
	int length = strlen(new_name);
	char* location = new_name;
	for (int i = 0; i < length; i++)
	{
		*location = tolower(*location);
		location++;
	}

	// make sure there isn't an entry with this name already
	if (doesExist(new_name))
	{
		MessageBox(NULL, "Cannot change name, entry with that name already exists", "Error", MB_OK);
		return;
	}

	// first create the new node
	SPRITENODE* temp = new SPRITENODE;
	memcpy(&temp->sprite, cur_sprite, sizeof(Sprite));
	strcpy(temp->name, new_name);

	// now add it to the linked list
	temp->prev = tail->prev;
	temp->next = tail;
	temp->prev->next = temp;
	tail->prev = temp;

	size++;

	mainWnd->GetLeftPane()->UpdateSpriteLibraryTree();
}

void SpriteLibrary::getSprite(char *name)
{
	SPRITENODE* temp = head->next;

	while (temp != tail)
	{
		if (strcmp(name, temp->name) == 0)
		{
			// create a new action
			Sprite* new_mouse_sprite = new Sprite;
			memcpy(new_mouse_sprite, &temp->sprite, sizeof(Sprite));

			UNDO_ACTION* action = new UNDO_ACTION;
			action->type = UT_SPRITE_CREATE;
			if (current_map->getMouseSprite() == NULL)
			{
				action->sprite_create.old_mouse_sprite = NULL;
			}
			else
			{
				memcpy(action->sprite_create.old_mouse_sprite, current_map->getMouseSprite(), sizeof(Sprite));
			}

			new_mouse_sprite->setVision(current_map->cur_vision);
			action->sprite_create.new_mouse_sprite = new_mouse_sprite;

			current_map->undo_buffer->addUndo(action);

			break;
		}

		temp = temp->next;
	}
}

void SpriteLibrary::getNextInitialize()
{
	current_sprite = head->next;
}

bool SpriteLibrary::getNext(char* text)
{
	if (current_sprite != tail)
	{
		strcpy(text, current_sprite->name);
		current_sprite = current_sprite->next;
		return true;
	}
	else
	{
		return false;
	}
}

void SpriteLibrary::removeSprite(char *name)
{
	SPRITENODE* temp = head->next;

	while (temp != tail)
	{
		if (strcmp(name, temp->name) == 0)
		{
			temp->prev->next = temp->next;
			temp->next->prev = temp->prev;
			delete temp;
			size--;
			mainWnd->GetLeftPane()->UpdateSpriteLibraryTree();
			break;
		}

		temp = temp->next;
	}
}

bool SpriteLibrary::doesExist(char* name)
{
	SPRITENODE* temp = head->next;

	while (temp != tail)
	{
		if (strcmp(name, temp->name) == 0)
		{
			return true;
		}

		temp = temp->next;
	}

	return false;
}

void SpriteLibrary::renameSprite(char* old_name, char* new_name)
{
	// make name lowercase first
	int length = strlen(new_name);
	char* location = new_name;
	for (int i = 0; i < length; i++)
	{
		*location = tolower(*location);
		location++;
	}
	
	// make sure there isn't an entry with this name already
	if (doesExist(new_name))
	{
		MessageBox(NULL, "Cannot change name, entry with that name already exists", "Error", MB_OK);
		return;
	}
	
	SPRITENODE* temp = head->next;

	while (temp != tail)
	{
		if (strcmp(old_name, temp->name) == 0)
		{
			strcpy(temp->name, new_name);
			mainWnd->GetLeftPane()->UpdateSpriteLibraryTree();
			break;
		}

		temp = temp->next;
	}
}

void SpriteLibrary::storeList()
{
	FILE *stream;

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", current_map->dmod_path, "library.dat");

	if ((stream = fopen(filename, "wb")) == NULL)
		return;

	fwrite(&size, sizeof(int), 1, stream);

	SPRITENODE* temp = head->next;

	while (temp != tail)
	{
		fwrite(temp->name, sizeof(char), MAX_SPRITE_NAME_LENGTH, stream);
		fwrite(&temp->sprite, sizeof(Sprite), 1, stream);

		temp = temp->next;
	}

	fclose(stream);
}

void SpriteLibrary::readList()
{
	FILE *stream;

	char filename[MAX_PATH];
	wsprintf(filename, "%s%s", current_map->dmod_path, "library.dat");

	if ((stream = fopen(filename, "rb")) == NULL)
		return;

	fread(&size, sizeof(int), 1, stream);

	SPRITENODE* prev;
	SPRITENODE* temp = head;

	for (int i = 0; i < size; i++)
	{
		prev = temp;
		temp = new SPRITENODE;
		fread(temp->name, sizeof(char), MAX_SPRITE_NAME_LENGTH, stream);
		fread(&temp->sprite, sizeof(Sprite), 1, stream);

		// now add it to the linked list
		temp->prev = prev;
		prev->next = temp;	
	}

	temp->next = tail;
	tail->prev = temp;

	fclose(stream);
}
