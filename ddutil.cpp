#include <StdAfx.h>

#include "Globals.h"
#include "Colors.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include "ddutil.h"
#include "Globals.h"
#include "Colors.h"

//-----------------------------------------------------------------------------
// Name: DDLoadBitmap()
// Desc: Create a DirectDrawSurface from a bitmap resource.
//-----------------------------------------------------------------------------

IDirectDrawSurface7* DDLoadBitmap(LPCSTR szBitmap, int& width, int& height)
{
	// open the bitmap file
	FILE* stream;
	if ( (stream = fopen(szBitmap, "rb")) == NULL)
		return NULL;

	// load the bitmap file
	IDirectDrawSurface7* pdds = loadBitmap(stream, width, height);

	// close the bitmap file
	fclose(stream);

	// return a pointer to the bitmap surface
	return pdds;
}

IDirectDrawSurface7* loadSurface(UCHAR *source_ptr, int width, int height)
{
	IDirectDrawSurface7* surface;
	DDSURFACEDESC2 ddsd; // create the surface description

	// set to access caps, width, and height
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize  = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	// set dimensions of the new bitmap surface
	ddsd.dwWidth  = width;		// width in pixels, not memory size
	ddsd.dwHeight = height;

	// allocate the video memory using the graphics manager
	surface = graphics_manager.allocateSurface(&ddsd);

	// lock the display surface
	surface->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	// find the destination bitmap pointer
	UCHAR *dest_ptr = (UCHAR *)ddsd.lpSurface;

	// iterate thru each scanline and copy bitmap
	for (int i = 0; i < height; i++)
	{
		// copy next line of data to destination
		memcpy(dest_ptr, source_ptr, width * colorformat);

		// advance pointers by one line
		dest_ptr   += ddsd.lPitch;
		source_ptr += width * colorformat;
	}
	// unlock the surface 
	surface->Unlock(NULL);

	return surface;
}

// returns 
IDirectDrawSurface7* loadBitmap(FILE *input_file, int& new_width, int& new_height)
{
	BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header
	BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
	PALETTEENTRY     palette[256];      // we will store the palette here

	//if ( (input_file = fopen(filename, "rb") ) == NULL)
	//	return false;

	// load the bitmap file header
	fread(&bitmapfileheader, 1, sizeof(BITMAPFILEHEADER), input_file);

	// load the bitmap info header
	fread(&bitmapinfoheader, 1, sizeof(BITMAPINFOHEADER), input_file);

	int height = new_height = bitmapinfoheader.biHeight;
	int width = new_width = bitmapinfoheader.biWidth;
	int bytes_per_pixel = bitmapinfoheader.biBitCount / 8;

	// calculate the offset due to the padding of each line (each line is always a multiple of 4 bytes wide)
	int bytes_per_line = width * bytes_per_pixel;
	int offset = 4 - (bytes_per_line % 4);

	if (offset == 4)
		offset = 0;

	int size = height * (bytes_per_line + offset);

	UCHAR *temp_buffer = new UCHAR[size];

	// only read in the pallette if this is an 8 bit source file
	if (bytes_per_pixel == 1)
	{
		// read in the pallette
		fread(&palette, 1, sizeof(PALETTEENTRY) * 256, input_file);
	}

	// read in the image data
	fread(temp_buffer, 1, size, input_file);

	// close the file
	fclose(input_file);

	// all relevant information has been extracted from the file into memory, now convert it to a nice form

	UCHAR *image_buffer = new UCHAR[width * height * colorformat];

	UCHAR* src_pixel;
	UCHAR* dest_pixel = image_buffer;

	if (colorformat == 2)
	{
		USHORT* dest_pixel2 = (USHORT*)(void*)dest_pixel;
		if (bytes_per_pixel == 1)
		{
			if (pixelformat == 565)
			{
				for (int y = 1; y <= height; y++)
				{
					src_pixel = temp_buffer + (bytes_per_line + offset) * (height - y);
					for (int x = 0; x < width; x++)
					{
						int index = int(*src_pixel);
						*dest_pixel2 = ((int(palette[index].peBlue) & 0xf8) << 8) + 
										((int(palette[index].peGreen) & 0xfc) << 3) +  
										 (int(palette[index].peRed) >> 3);
						dest_pixel2++;
						src_pixel++;
					}
				}
			}
			else
			{
				for (int y = 1; y <= height; y++)
				{
					src_pixel = temp_buffer + (bytes_per_line + offset) * (height - y);
					for (int x = 0; x < width; x++)
					{
						int index = int(*src_pixel);
						*dest_pixel2 = ((int(palette[index].peBlue) & 0xf8) << 7) + 
										((int(palette[index].peGreen) & 0xf8) << 2) +  
										 (int(palette[index].peRed) >> 3);
						dest_pixel2++;
						src_pixel++;
					}
				}
			}
		}
		else if (bytes_per_pixel == 3)
		{
			if (pixelformat == 565)
			{
				for (int y = 1; y <= height; y++)
				{
					src_pixel = temp_buffer + (bytes_per_line + offset) * (height - y);
					for (int x = 0; x < width; x++)
					{
						*dest_pixel2 = ((int( *(src_pixel + 2) ) & 0xf8) << 8) + 
									  ((int( *(src_pixel + 1) ) & 0xfc) << 3) +  
									   (int( *(src_pixel + 0) ) >> 3);
						dest_pixel2++;
						src_pixel += 3;
					}
				}
			}
			else
			{
				for (int y = 1; y <= height; y++)
				{
					src_pixel = temp_buffer + (bytes_per_line + offset) * (height - y);
					for (int x = 0; x < width; x++)
					{
						*dest_pixel2 = ((int( *(src_pixel + 2) ) & 0xf8) << 7) + 
									  ((int( *(src_pixel + 1) ) & 0xf8) << 2) +  
									   (int( *(src_pixel + 0) ) >> 3);
						dest_pixel2++;
						src_pixel += 3;
					}
				}
			}	
		}
		else
		{
			// bitmap is not 8 bit or 24 bit
			delete [] temp_buffer;
			delete [] image_buffer;
			return NULL;
		}
	}
	else if (colorformat == 3)
	{
		if (bytes_per_pixel == 1)
		{
			// image is 24 bit
			// just need to flip the pixels around
			for (int y = 1; y <= height; y++)
			{
				src_pixel = temp_buffer + (bytes_per_line + offset) * (height - y);
				for (int x = 0; x < width; x++)
				{
					int index = int(*src_pixel);
					*dest_pixel = palette[index].peRed;
					dest_pixel++;
					*dest_pixel = palette[index].peGreen;
					dest_pixel++;
					*dest_pixel = palette[index].peBlue;
					dest_pixel++;

					src_pixel++;
				}
			}
		}
		else if (bytes_per_pixel == 3)
		{
			// image is 24 bit
			// just need to flip the pixels around
			for (int y = 1; y <= height; y++)
			{
				src_pixel = temp_buffer + (bytes_per_line + offset) * (height - y);
				for (int x = 0; x < width; x++)
				{
					*dest_pixel = *(src_pixel + 0);
					dest_pixel++;
					*dest_pixel = *(src_pixel + 1);
					dest_pixel++;
					*dest_pixel = *(src_pixel + 2);
					dest_pixel++;

					src_pixel += 3;
				}
			}
		}
		else
		{
			delete [] temp_buffer;
			delete [] image_buffer;
			return NULL;
		}
	}
	else if (colorformat == 4)
	{
		if (bytes_per_pixel == 1)
		{
			// image is 24 bit
			// just need to flip the pixels around
			for (int y = 1; y <= height; y++)
			{
				src_pixel = temp_buffer + (bytes_per_line + offset) * (height - y);
				for (int x = 0; x < width; x++)
				{
					int index = int(*src_pixel);
					
					*dest_pixel = palette[index].peRed;
					dest_pixel++;
					*dest_pixel = palette[index].peGreen;
					dest_pixel++;
					*dest_pixel = palette[index].peBlue;
					dest_pixel++;
					*dest_pixel = 0;
					dest_pixel++;

					src_pixel++;
				}
			}
		}
		else if (bytes_per_pixel == 3)
		{
			// image is 24 bit
			// just need to flip the pixels around
			for (int y = 1; y <= height; y++)
			{
				src_pixel = temp_buffer + (bytes_per_line + offset) * (height - y);
				for (int x = 0; x < width; x++)
				{
					
					*dest_pixel = *(src_pixel + 0);
					dest_pixel++;
					*dest_pixel = *(src_pixel + 1);
					dest_pixel++;
					*dest_pixel = *(src_pixel + 2);
					dest_pixel++;
					*dest_pixel = 0;
					dest_pixel++;

					src_pixel += 3;
				}
			}
		}
		else
		{
			delete [] temp_buffer;
			delete [] image_buffer;
			return NULL;
		}
	}

	delete [] temp_buffer;

	// all image processing is done, now the file can be loaded into a surface
	IDirectDrawSurface7* surface = loadSurface(image_buffer, width, height);

	delete [] image_buffer;

	return surface;
}