#ifndef _DDUTIL_H_
#define _DDUTIL_H_

#include <ddraw.h>

IDirectDrawSurface7 *DDLoadBitmap(LPCSTR szBitmap, int& width, int& height);
IDirectDrawSurface7* loadBitmap(FILE *input_file, int& new_width, int& new_height);

#endif
