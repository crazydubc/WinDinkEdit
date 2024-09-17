#ifndef _TOOLS_H_
#define _TOOLS_H_


// .c -> .d stuff
#define MIN_BUFFER_SIZE		1024
#define BIG_BUFFER_SIZE		(1 << 16)
#define TABLE_SIZE			128
#define MINI_BUFFER_SIZE	(1 << 12)

void compressAll(BOOL delete_file, BOOL remove_comments);


// ffcreate stuff
#define MAX_BMP_LENGTH	13
#define DATA_BUFFER_SIZE	1024

void ffcreate(char* path, bool delete_file);

// screenshot stuff
int printScreen(char *filename, LPDIRECTDRAWSURFACE7 surface);

#endif