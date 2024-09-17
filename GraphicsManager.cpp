// INCLUDES ///////////////////////////////////////////////

#include "StdAfx.h"

#include "Globals.h"
#include "GraphicsManager.h"
#include "Colors.h"
#include "Engine.h"

// DEFINES ////////////////////////////////////////////////

// MACROS /////////////////////////////////////////////////

// TYPES //////////////////////////////////////////////////

// PROTOTYPES /////////////////////////////////////////////

// GLOBALS ////////////////////////////////////////////////

GraphicsManager::GraphicsManager()
{
	videoMemoryUsed = 0;
	videoCardOnlyMemory = false;

	head = new GRAPHICQUE;
	tail = new GRAPHICQUE;

	head->next = tail;
	tail->prev = head;	
}

GraphicsManager::~GraphicsManager()
{
	empty();

	if (head)
	{
		delete head;
		head = NULL;
	}

	if (tail)
	{
		delete tail;
		tail = NULL;
	}
}

void GraphicsManager::empty()
{
	// if empty has already been called exit
	if (head == NULL || tail == NULL)
		return;

	if (head->next != tail)
	{
		GRAPHICQUE* temp = head->next->next;

		while (temp != tail)
		{
			delete temp->prev;

			temp = temp->next;
		}
		delete temp->prev;
	}

	head->next = tail;
	tail->prev = head;
}

IDirectDrawSurface7* GraphicsManager::allocateSurface(DDSURFACEDESC2 *ddsd)
{
	IDirectDrawSurface7 *surface = NULL;

	if (videoCardOnlyMemory)
	{
		ddsd->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;

		HRESULT hr;
		// create the surface
		
		while (true)
		{
			if (FAILED(hr = lpdd7->CreateSurface(ddsd, &surface, NULL)))
			{
				if (hr == DDERR_OUTOFVIDEOMEMORY)
				{
					if (removeGraphics(500000))
						continue;
				}
			}
			break;
		}
	}
	
	if (surface == NULL)
	{
		ddsd->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

		if (lpdd7->CreateSurface(ddsd, &surface, NULL) != DD_OK)
			return NULL;
	}

	videoMemoryUsed += ddsd->dwWidth * ddsd->dwHeight * colorformat;

	return surface;
}

GRAPHICQUE* GraphicsManager::addGraphic(LPDIRECTDRAWSURFACE7* surface)
{
	// fill in the new graphic que structure
	GRAPHICQUE* temp = new GRAPHICQUE;
	temp->surface = surface;

	temp->prev = tail->prev;

	// set the old tails next to the new soon to be tail
	tail->prev->next = temp;

	// tail->prev should now equal the latest entry
	tail->prev = temp;
	temp->next = tail;

	return temp;
}

bool GraphicsManager::moveLocation(GRAPHICQUE* location)
{
	// delete old references
	location->prev->next = location->next;
	location->next->prev = location->prev;

	// create new references
	location->prev = tail->prev;
	location->next = tail;

	// set the old tails next to the new soon to be tail
	tail->prev->next = location;

	// tail should now equal the latest entry
	tail->prev = location;

	return true;
}

// removes a graphic from the head of the list
bool GraphicsManager::removeGraphic()
{
	if (head->next == tail)
		return false;

	LPDIRECTDRAWSURFACE7* surface = head->next->surface;

	if (*surface)
	{
		DDSURFACEDESC2 ddsd;
		memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		(*surface)->GetSurfaceDesc(&ddsd);

		videoMemoryUsed -= colorformat * ddsd.dwWidth * ddsd.dwHeight;

		(*surface)->Release();
		*surface = NULL;
	}

	GRAPHICQUE* temp = head->next->next;

	temp->prev = head;

	// delete old graphic record
	delete head->next;

	head->next = temp;

	return true;
}

// removes all graphics that reside in video memory
bool GraphicsManager::removeAllVideoMemGraphics()
{
	GRAPHICQUE* temp = head->next;
	DDSURFACEDESC2 ddsd;
	GRAPHICQUE* temp2;

	LPDIRECTDRAWSURFACE7* surface;

	while (temp != tail)
	{
		temp2 = temp->next;
		surface = temp->surface;

		memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		(*surface)->GetSurfaceDesc(&ddsd);

		if ((ddsd.ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM) > 0)
		{
			if (*surface)
			{
				videoMemoryUsed -= colorformat * ddsd.dwWidth * ddsd.dwHeight;
				(*surface)->Release();
				*surface = NULL;
			}

			// delete old references
			temp->prev->next = temp->next;
			temp->next->prev = temp->prev;

			delete temp;
		}

		temp = temp2;
	}

	return true;
}

bool GraphicsManager::removeGraphics(int amount)
{
	DDSCAPS2 ddsCaps2; 
	DWORD total_video_memory; 
	DWORD free_video_memory; 
	HRESULT hr; 

	if (!videoCardOnlyMemory)
	{
		int target_memory = videoMemoryUsed - amount;

		if (target_memory < 0)
			target_memory = 0;	// ERROR

		while (target_memory < videoMemoryUsed)
		{
			if (removeGraphic() == false)
				return false;
		}
		return true;
	}

	// Get total and available video memory for Direct Draw
	// Initialize the structure.
	memset(&ddsCaps2, 0, sizeof(ddsCaps2));

	// Set caps to indicate we want to know about video memory
	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY;

	// The GetAvailableVidMem function if sucessful will put the total amount of video memory
	// that DirectX can use for anything in total_memory_for_textures, and will put the total - used amount in
	// free_memory_for_textures.
	hr = lpdd7->GetAvailableVidMem(&ddsCaps2, &total_video_memory, &free_video_memory);

	DWORD begin_memory = free_video_memory;

	while (free_video_memory < begin_memory + amount)
	{
		if (removeGraphic() == false)
			return false;
		hr = lpdd7->GetAvailableVidMem(&ddsCaps2, &total_video_memory, &free_video_memory);
	}

	return true;
}

bool GraphicsManager::getVideoMemory()
{
	DDSCAPS2 ddsCaps2; 
	DWORD total_memory_for_textures; 
	DWORD free_memory_for_textures; 
	DWORD total_video_memory; 
	DWORD free_video_memory; 
	HRESULT hr; 

	// Get total and available video and / or system memory for textures
	// Initialize the structure.
	memset(&ddsCaps2, 0, sizeof(ddsCaps2));

	// Set caps to indicate we want to know about texture memory
	ddsCaps2.dwCaps = DDSCAPS_TEXTURE;

	// The GetAvailableVidMem function if sucessful will put the total amount of video + system memory
	// that DirectX can use for textures in total_memory_for_textures, and will put the total - used amount in
	// free_memory_for_textures.
	hr = lpdd7->GetAvailableVidMem(&ddsCaps2, &total_memory_for_textures, &free_memory_for_textures);

	// Get total and available video memory for Direct Draw
	// Initialize the structure.
	memset(&ddsCaps2, 0, sizeof(ddsCaps2));

	// Set caps to indicate we want to know about video memory
	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY;

	// The GetAvailableVidMem function if sucessful will put the total amount of video memory
	// that DirectX can use for anything in total_memory_for_textures, and will put the total - used amount in
	// free_memory_for_textures.
	hr = lpdd7->GetAvailableVidMem(&ddsCaps2, &total_video_memory, &free_video_memory);


	int min_ram = MIN_VIDEO_RAM_FOR_USING_ONLY_VIDEO_MEMORY;
	if (colorformat > 2)
		min_ram *= 2;

	if (free_video_memory > min_ram)
		videoCardOnlyMemory = true;

	return true;
}

bool GraphicsManager::checkMemory()
{
	if (!videoCardOnlyMemory)
	{
		if (videoMemoryUsed > 16000000)
		{
			if (removeGraphics(videoMemoryUsed - 16000000) == false)
				return false;
		}
	}
	return true;
}