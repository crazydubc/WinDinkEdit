#ifndef _GRAPHICS_MANAGER_H_
#define _GRAPHICS_MANAGER_H_

#include "Globals.h"
#include <ddraw.h>

#define MIN_VIDEO_RAM_FOR_USING_ONLY_VIDEO_MEMORY	3000000

struct GRAPHICQUE
{
	GRAPHICQUE* next;
	GRAPHICQUE* prev;
	LPDIRECTDRAWSURFACE7* surface;
};

class GraphicsManager
{
public:
	GraphicsManager();
	~GraphicsManager();

	GRAPHICQUE* addGraphic(LPDIRECTDRAWSURFACE7* surface);
	bool moveLocation(GRAPHICQUE* location);
	bool removeAllVideoMemGraphics();
	bool removeGraphics(int amount);
	IDirectDrawSurface7 *allocateSurface(DDSURFACEDESC2 *ddsd);
	bool checkMemory();
	bool getVideoMemory();
	void empty();

	bool videoCardOnlyMemory;	

private:
	bool removeGraphic();
	int videoMemoryUsed;

	GRAPHICQUE* head;
	GRAPHICQUE* tail;
};

#endif