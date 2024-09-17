#include "StdAfx.h"

#include <windows.h>
#include <stdio.h>

#include "Globals.h"
#include "Map.h"
#include "Tools.h"
#include <io.h>
#include <SYS\STAT.H>

// prints the destination screen to the specified .bmp file in 24 bit color
int printScreen(char *filename, LPDIRECTDRAWSURFACE7 surface)
{
	DDSURFACEDESC2 ddsCaps2;
	memset(&ddsCaps2, 0, sizeof(ddsCaps2));
	ddsCaps2.dwSize = sizeof(DDSURFACEDESC2);
	ddsCaps2.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_PITCH | DDSD_LPSURFACE;

	HRESULT hr = surface->GetSurfaceDesc(&ddsCaps2);

	int width = ddsCaps2.dwWidth;
	int height = ddsCaps2.dwHeight;

	int colorDepth;		// 2, 3, or 4 bytes
	int pixelFormat;	// used with 16 bit color
	int temp = 3;

	if (ddsCaps2.ddpfPixelFormat.dwRGBBitCount == 16)	// 
	{
		colorDepth = 2;
		if (ddsCaps2.ddpfPixelFormat.dwGBitMask == 0x07e0)	// 5 - 6 - 5 pixel format
		{
			pixelFormat = 565;
		}
		else if (ddsCaps2.ddpfPixelFormat.dwGBitMask == 0x03e0)	// 5 - 5 - 5 pixel format
		{
			pixelFormat = 555;
		}
	}
	else if (ddsCaps2.ddpfPixelFormat.dwRGBBitCount == 24)
	{
		colorDepth = 3;
	}
	else if (ddsCaps2.ddpfPixelFormat.dwRGBBitCount == 32)
	{
		colorDepth = 4;
		temp = 4;
	}
	else
	{
		return false;
	}

	//int offset = width % 4;	// bmp files need padding

	int image_buffer_size = height * (width) * temp;
	int bytes_per_line = width * 3;
	int imageSize = height * bytes_per_line;
	
	int final_image_size = imageSize + height;

	// allocate the image buffer
	UCHAR *image = new UCHAR[image_buffer_size];
	// allocate the secondary image buffer
	UCHAR *buffer = new UCHAR[image_buffer_size];

	// lock the display surface
	surface->Lock(NULL,
				&ddsCaps2,
				DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
				NULL);

	// find the source bitmap pointer
	UCHAR *source_ptr = (UCHAR *)ddsCaps2.lpSurface;
	UCHAR *dest_ptr = image;

	// iterate thru each scanline and copy bitmap
	for (int row = 0; row < height; row++)
	{
		// copy next line of data to destination
		memcpy(dest_ptr, source_ptr, width * colorDepth);

		// advance pointers by one line
		dest_ptr	+= width * colorDepth;
		source_ptr	+= ddsCaps2.lPitch;
	}
	// unlock the surface 
	surface->Unlock(NULL);

	if (colorDepth == 2)
	{
		if (pixelFormat == 565 )
		{
			// 5 - 6 - 5 pixel format
			for (int i = 0; i < width * height; i++)
			{
				// extract RGB components (in BGR order), note the scaling
				buffer[i*3 + 2] = image[i*2 + 1] & 0xf8;	// red
				buffer[i*3 + 1] = (((image[i*2 + 1] & 0x07) << 5) | ((image[i*2 + 0] >> 6) << 3) );	// green
				buffer[i*3 + 0] = (image[i*2 + 0] & 0x3e) << 3;	// blue
			}
		}
		else
		{
			// 5 - 5 - 5 pixel format
			for (int i = 0; i < width * height; i++)
			{
				// extract RGB components (in BGR order), note the scaling
				buffer[i*3 + 2] = (image[i*2 + 1] & 0x7c) << 1;	// red
				buffer[i*3 + 1] = (((image[i*2 + 1] & 0x03) << 6) | ((image[i*2 + 0] >> 5) << 3) );	// green
				buffer[i*3 + 0] = (image[i*2 + 0] & 0x1e) << 3;	// blue
			}
		}
	}
	else if (colorDepth == 3)
	{
		memcpy(image, buffer, imageSize);
	}
	else if (colorDepth == 4)
	{
		source_ptr = image;
		dest_ptr = buffer;

		for (int i = 0; i < width * height; i++)
		{
			memcpy(dest_ptr, source_ptr, 3);
			source_ptr += 4;
			dest_ptr += 3;
		}
	}

	// flip vertically
	source_ptr = buffer;
	dest_ptr = image + (height - 1) * (bytes_per_line);

	for (int i = 0; i < height; i++)
	{
		memcpy(dest_ptr, source_ptr, bytes_per_line);

		source_ptr += bytes_per_line;
		dest_ptr -= (bytes_per_line);
	}

	// release the temporary image buffer
	delete [] buffer;


	BITMAPFILEHEADER bitmapfileheader;  // this contains the bitmapfile header

	// fill in the bitmap fileheader
	// 14 bytes long
	bitmapfileheader.bfType = 'MB';		// Indicates the type of the file and is always set to BM
	bitmapfileheader.bfSize = 54 + final_image_size;// Specifies the size of the whole file in bytes
	bitmapfileheader.bfReserved1 = 0;	// Reserved -- must be set to 0
	bitmapfileheader.bfReserved2 = 0;	// Reserved -- must be set to 0
	bitmapfileheader.bfOffBits = 54;		// Specifies the byte offset from the BitmapFileHeader to the start of the image

	BITMAPINFOHEADER bitmapinfoheader;  // this is all the info including the palette
	// fill in the bitmap infoheader
	// 40 bytes long
	bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);	// Specifies the number of bytes required by the BITMAPINFOHEADER structure.
	bitmapinfoheader.biWidth = width;	// Specifies the width of the bitmap in pixels
	bitmapinfoheader.biHeight = height;	// Specifies the height of the bitmap in pixels
	bitmapinfoheader.biPlanes = 1;				// Specifies the number of planes for the target device. This member must be set to 1
	bitmapinfoheader.biBitCount = 24;	// Specifies the number of bits per pixel. This value must be 1, 4, 8, or 24
	bitmapinfoheader.biCompression = 0;			// Specifies the type of compression for a compressed bitmap. In a 24-bit format, the variable is set to 0
	bitmapinfoheader.biSizeImage = final_image_size;	// Specifies the size in bytes of the image. It is valid to set this member to 0 if the bitmap is in the BI_RGB format
	bitmapinfoheader.biXPelsPerMeter = 0;		// Specifies the horizontal resolution, in pixels per meter, of the target device for the bitmap
	bitmapinfoheader.biYPelsPerMeter = 0;		// Specifies the vertical resolution, in pixels per meter, of the target device for the bitmap
	bitmapinfoheader.biClrUsed = 0;				// Number of palettes used (if less than standard)
	bitmapinfoheader.biClrImportant = 0;		// Specifies the number of color indexes considered important for displaying the bitmap

	FILE *stream;

	// open the file if it exists
	if ( (stream = fopen(filename, "wb") ) == NULL)
		return false;

	// now write the bitmap file header
	fwrite( &bitmapfileheader, 1, sizeof(BITMAPFILEHEADER), stream );

	// now write the bitmap info header
	fwrite( &bitmapinfoheader, 1, sizeof(BITMAPINFOHEADER), stream );

	// now write the image out
	fwrite( image, 1, imageSize, stream);

	// close the file
	fclose(stream);

	// release the final image buffer
	delete [] image;

	return true;
}

struct FILENAME
{
	int file_location;
	char filename[13];
};

// compare function used for quicksort
int compare_filenames( const void *arg1, const void *arg2 )
{
   /* Compare all of both strings: */
   return _stricmp( (*(FILENAME*)arg1).filename, (*(FILENAME*)arg2).filename );
}


void ffcreate(char* path, bool delete_file)
{
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFiles = TRUE;
	int i;

	int num_files = 0;

	char buffer[250];
	strcpy(buffer, path);
	strcat(buffer, "*.bmp");

	hFind = FindFirstFile(buffer, &dataFind);

	FILENAME* file_list = NULL;

	// get all the *.bmp files in the directory
	while(hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		if (strlen(dataFind.cFileName) < MAX_BMP_LENGTH)
		{
			file_list = (FILENAME*)realloc(file_list, sizeof(FILENAME) * (num_files + 1));
			memset(&file_list[num_files], 0 , sizeof(FILENAME));

			strcpy(file_list[num_files].filename, dataFind.cFileName);

			num_files++;
		}

		bMoreFiles = FindNextFile(hFind, &dataFind);
	}

	FindClose(hFind);

	strcpy(buffer, path);
	strcat(buffer, "*.*");

	hFind = FindFirstFile(buffer, &dataFind);
	bMoreFiles = TRUE;

	// get all the directories
	while (hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		if ((dataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 && dataFind.cFileName[0] != '.')
		{
			strcpy(buffer, path);
			strcat(buffer, dataFind.cFileName);
			strcat(buffer, "\\");

			ffcreate(buffer, delete_file);
		}

		bMoreFiles = FindNextFile(hFind, &dataFind);
	}

	FindClose(hFind);

	if (num_files == 0)
		return;


	////////////// Sort the filenames alphabetically
	qsort(file_list, num_files, sizeof(FILENAME), compare_filenames);


	FILE *output, *input;

	strcpy(buffer, path);
	strcat(buffer, "dir.ff");

	if ((output = fopen(buffer, "wb")) == NULL)
	{
		return;
	}

	int temp = num_files + 1;

	fwrite(&temp, sizeof(int), 1, output);

	FILENAME dummy;
	memset(&dummy, 0, sizeof(FILENAME));
	for (i = 0; i < num_files + 1; i++)
		fwrite(&dummy, 17, 1, output);

	UCHAR data_buffer[DATA_BUFFER_SIZE];

	int file_offset_location = 4 + (num_files + 1) * 17;

	for (i = 0; i < num_files; i++)
	{
		strcpy(buffer, path);
		strcat(buffer, file_list[i].filename);

		file_list[i].file_location = file_offset_location;

		if ((input = fopen(buffer, "rb")) == NULL)
		{
			fclose(output);
			return;
		}

		int amount_read;

		while ((amount_read = fread(data_buffer, 1, DATA_BUFFER_SIZE, input)) != 0)
		{
			file_offset_location += amount_read;
			fwrite(data_buffer, 1, amount_read, output);
		}

		fclose(input);

		// delete file if option has been selected
		if (delete_file)
		{
			// remove read only attribute
			_chmod(buffer, _S_IREAD | _S_IWRITE);
			remove(buffer);
		}
	}

	// go to fourth byte in file
	fseek(output, sizeof(int), SEEK_SET);

	for (i = 0; i < num_files; i++)
	{
		fwrite(&file_list[i], 17, 1, output);
	}

	dummy.file_location = file_offset_location;

	fwrite(&dummy, sizeof(FILENAME), 1, output);

	fclose(output);

	if (file_list)
		delete [] file_list;
}

bool compact(char *file_name, BOOL remove_comments)
{
	int big_buffer[BIG_BUFFER_SIZE];
	int mini_buffer[MINI_BUFFER_SIZE];
	int i;

	UCHAR left[TABLE_SIZE], right[TABLE_SIZE];

// read in the file
	FILE *stream;

	if ((stream = fopen(file_name, "rb")) == NULL)
	{
		return false;
	}

	int buf_size = 0;
	UCHAR *buffer = NULL;
	int amount_read;
	int total_amount_read = 0;

	do
	{
		buf_size += MIN_BUFFER_SIZE;
		buffer = (UCHAR*)realloc(buffer, buf_size);
		amount_read = fread(buffer + total_amount_read, 1, MIN_BUFFER_SIZE, stream);
		total_amount_read += amount_read;
	} while (amount_read == MIN_BUFFER_SIZE);

	fclose(stream);

// remove all comments?
	int j = 0;

	if (remove_comments)
	{
		for (i = 0; j < total_amount_read - 1; i++)
		{
			if (buffer[i] == '/' && buffer[i + 1] == '/')
			{
				i += 2;
				total_amount_read -= 2;
				// skip the rest of the line
				while (j < total_amount_read)
				{
					if (buffer[i] == 13 || buffer[i] == '\n')
					{
						i--;
						break;
					}
					i++;
					total_amount_read--;
				}
			}
			else
			{
				buffer[j] = buffer[i];
				j++;
			}
		}
	}

	if (j < total_amount_read)
	{
		buffer[j] = buffer[i];
	}

// create the table now

	int max_value;
	int max_value_location;
	int used_pairs = 0;

	UCHAR left_char, right_char;

	int location;
	int* buf_ptr;
	UCHAR* dest_ptr, *src_ptr;
	UCHAR newchar;
	
	// clear the big buffer
	buf_ptr = big_buffer;
	for (i = 0; i < BIG_BUFFER_SIZE / 4; i++)
	{
		// unroll the loop, makes it faster, possibly at least
		*buf_ptr = 0;
		buf_ptr++;
		*buf_ptr = 0;
		buf_ptr++;
		*buf_ptr = 0;
		buf_ptr++;
		*buf_ptr = 0;
		buf_ptr++;
	}

	// clear the small buffer
	buf_ptr = mini_buffer;
	for (i = 0; i < MINI_BUFFER_SIZE / 4; i++)
	{
		*buf_ptr = 0;
		buf_ptr++;
		*buf_ptr = 0;
		buf_ptr++;
		*buf_ptr = 0;
		buf_ptr++;
		*buf_ptr = 0;
		buf_ptr++;
	}	

	// count the instances of each pair
	src_ptr = buffer;
	for (i = 0; i < total_amount_read - 1; i++)
	{
		location = ((int)*src_ptr << 8) + (int)*(src_ptr + 1);

		big_buffer[location]++;
		mini_buffer[location >> 4]++;
		src_ptr++;
	}

	// create the table and compress
	for (used_pairs = 0; used_pairs < TABLE_SIZE - 1; used_pairs++)
	{
		max_value = 0;
		buf_ptr = mini_buffer;
		
		// find the most common pair now
		for (i = 0; i < MINI_BUFFER_SIZE; i++)
		{
			if (*buf_ptr > max_value)
			{
				for (int x = 0; x < 16; x++)
				{
					location = (i << 4) + x;
					if (big_buffer[location] > max_value)
					{
						max_value = big_buffer[location];
						max_value_location = location;
					}
				}
			}
			buf_ptr++;
		}

		// add the pair to the table
		if (max_value > 2)
		{
			left_char = left[used_pairs] = max_value_location >> 8;
			right_char = right[used_pairs] = max_value_location & 0xFF;
		}
		else
			break;

		// replace all instances of that pair in the buffer
		src_ptr = buffer;
		dest_ptr = buffer;
		newchar = used_pairs + TABLE_SIZE;
		for (i = 0; i < total_amount_read - 1; i++)
		{
			if (*src_ptr == left_char && *(src_ptr + 1) == right_char)
			{
				// remove left pair
				if (i != 0)
				{
					location = (*(src_ptr - 1) << 8) + *src_ptr;
					big_buffer[location]--;
					mini_buffer[location >> 4]--;

					// add new pair
					location = (*(src_ptr - 1) << 8) + newchar;
					big_buffer[location]++;
					mini_buffer[location >> 4]++;
				}
				// remove right pair
				if (i != total_amount_read - 2)
				{
					location = (*(src_ptr + 1) << 8) + *(src_ptr + 2);
					big_buffer[location]--;
					mini_buffer[location >> 4]--;

					// add new pair
					location = (newchar << 8) + *(src_ptr + 2);
					big_buffer[location]++;
					mini_buffer[location >> 4]++;
				}

				*dest_ptr = newchar;
				src_ptr++;
				total_amount_read--;
			}
			else
			{
				*dest_ptr = *src_ptr;
			}
			src_ptr++;
			dest_ptr++;
		}

		if (i == total_amount_read - 1)
			*dest_ptr = *src_ptr;

		// remove middle pairs
		big_buffer[max_value_location] = 0;
		mini_buffer[max_value_location >> 4] -= max_value;
	}

// write out the file now
	file_name[strlen(file_name) - 1] = 'd';

	if ((stream = fopen(file_name, "wb")) == NULL)
	{
		delete [] buffer;
		return false;
	}

	UCHAR num_pairs = used_pairs + TABLE_SIZE;

	fwrite(&num_pairs, 1 , 1, stream);

	for (i = 0; i < used_pairs; i++)
	{
		fwrite(&left[i], 1 , 1, stream);
		fwrite(&right[i], 1 , 1, stream);
	}

	fwrite(buffer, 1, total_amount_read, stream);

	fclose(stream);

	if (buffer)
		delete [] buffer;

	return true;
}

void compressAll(BOOL delete_file, BOOL remove_comments)
{
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFiles = TRUE;

	char buffer[250];
	wsprintf(buffer, "%sstory\\*.c", current_map->dmod_path);

	hFind = FindFirstFile(buffer, &dataFind);

	// get all the *.c files in the directory
	while(hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		wsprintf(buffer, "%sstory\\%s", current_map->dmod_path, dataFind.cFileName);
		if (compact(buffer, remove_comments) == true)
		{
			if (delete_file)
			{
				wsprintf(buffer, "%sstory\\%s", current_map->dmod_path, dataFind.cFileName);
				remove(buffer);
			}
		}

		bMoreFiles = FindNextFile(hFind, &dataFind);
	}

	FindClose(hFind);
}