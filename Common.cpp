// INCLUDES ///////////////////////////////////////////////

#include "StdAfx.h"

#include "Common.h"
#include "Globals.h"
#include "Map.h"
#include "Colors.h"
#include "minimap.h"

#include "mainfrm.h"
#include "GEtFolderDlg.h"

#include <fstream>
using namespace std;

#define MAX_DMOD_NAME_LENGTH	20

CString getDmodName(CString dmod_path)
{
	char* dmod_name;
	char* next_token = dmod_name = strtok(dmod_path.GetBuffer(0), "\\");
	if (dmod_name == NULL)
		return "";

	while (next_token != NULL)
	{
		dmod_name = next_token;
		next_token = strtok(NULL, "\\");
	}

	return dmod_name;
}

// reads the windinkedit ini file
bool readWDEIni()
{
	char default_string[256];
	char return_string[256];
	char file_path[MAX_PATH];
	char windows_path[MAX_PATH];

	if (GetWindowsDirectory(windows_path, MAX_PATH) == NULL)
		return false;

	wsprintf(file_path, "%s\\prefs.ini", windows_path);

	// screen gap
	wsprintf(default_string, "%d", DEFAULT_SCREEN_GAP);
	if( GetPrivateProfileString("Display_Settings",
		"screen_gap", default_string, return_string, 256, file_path) <= 0)
		return false;

	screen_gap = atoi(return_string);

	//auto update minimap
	wsprintf(default_string, "%d", 0);
	if( GetPrivateProfileString("Display_Settings",
		"update_minimap", default_string, return_string, 256, file_path) <= 0)
		return false;

	update_minimap = atoi(return_string);

	// max undo level
	wsprintf(default_string, "%d", DEFAULT_MAX_UNDO_LEVEL);
	if( GetPrivateProfileString("General_Settings",
		"Max_Undo_Level", default_string, return_string, 256, file_path) <= 0)
		return false;

	max_undo_level = atoi(return_string);

	// auto save time
	wsprintf(default_string, "%d", DEFAULT_AUTOSAVE_TIME);
	if( GetPrivateProfileString("General_Settings",
		"Auto_Save_Time", default_string, return_string, 256, file_path) <= 0)
		return false;

	auto_save_time = atoi(return_string);

	//Display Settings
	wsprintf(default_string, "%d", true);
	if( GetPrivateProfileString("Display_Settings",
		"show_minimap_progress", default_string, return_string, 256, file_path) <= 0)
		return false;

	show_minimap_progress = atoi(return_string);

	wsprintf(default_string, "%d", false);
	if(GetPrivateProfileString("General_Settings",
		"Optimize_Dink_Ini", default_string, return_string, 256, file_path) <= 0)
		return false;

	optimize_dink_ini = atoi(return_string);

	wsprintf(default_string, "%i", 1);
	if(GetPrivateProfileString("General_Settings",
		"Tile_Brush_Size", default_string, return_string, 256, file_path) <= 0)
		return false;

	tile_brush_size = atoi(return_string);

	wsprintf(default_string, "%i", fast_minimap_update);
	if(GetPrivateProfileString("Display_Settings",
		"Fast_Minimap_Update", default_string, return_string, 256, file_path) <= 0)
		return false;

	fast_minimap_update = atoi(return_string);

	wsprintf(default_string, "%i", hover_sprite_info);
	if(GetPrivateProfileString("General_Settings",
		"Hover_Sprite_Info", default_string, return_string, 256, file_path) <= 0)
		return false;

	hover_sprite_info = atoi(return_string);

	wsprintf(default_string, "%i", hover_sprite_hardness);
	if(GetPrivateProfileString("General_Settings",
		"Hover_Sprite_Hardness", default_string, return_string, 256, file_path) <= 0)
		return false;

	hover_sprite_hardness = atoi(return_string);

	wsprintf(default_string, "%i", help_text);
	if (GetPrivateProfileString("General_Settings",
		"Show_Help_Text", default_string, return_string, 256, file_path) <= 0)
		return false;

	help_text = atoi(return_string);
	
	return true;
}

// writes the windinkedit ini file
bool writeWDEIni()
{
	char default_string[256];
	char file_path[MAX_PATH];
	char windows_path[MAX_PATH];

	if (GetWindowsDirectory(windows_path, MAX_PATH) == NULL)
		return false;

	wsprintf(file_path, "%s\\prefs.ini", windows_path);

	// screen gap
	wsprintf(default_string, "%d", screen_gap);
	if (WritePrivateProfileString("Display_Settings", 
		"screen_gap", default_string, file_path) == NULL)
		return false;
	
	//auto update minimap
	wsprintf(default_string, "%d", update_minimap);
	if (WritePrivateProfileString("Display_Settings", 
		"update_minimap", default_string, file_path) == NULL)
		return false;

	//display minimap update progress
	wsprintf(default_string, "%d", show_minimap_progress);
	if (WritePrivateProfileString("Display_Settings",
		"show_minimap_progress", default_string, file_path) == NULL)
		return false;

	// max undo level
	wsprintf(default_string, "%d", max_undo_level);
	if (WritePrivateProfileString("General_Settings", 
		"Max_Undo_Level", default_string, file_path) == NULL)
		return false;

	// max undo level
	wsprintf(default_string, "%d", auto_save_time);
	if (WritePrivateProfileString("General_Settings", 
		"Auto_Save_Time", default_string, file_path) == NULL)
		return false;

	//optimize dink ini
	wsprintf(default_string, "%d", optimize_dink_ini);
	if (WritePrivateProfileString("General_Settings",
		"Optimize_Dink_Ini", default_string, file_path) == NULL)
		return false;

	//Brush size for hard tiles
	wsprintf(default_string, "%i", tile_brush_size);
	if (WritePrivateProfileString("General_Settings",
		"Tile_Brush_Size", default_string, file_path) == NULL)
		return false;

	//fast minimap updater
	wsprintf(default_string, "%i", fast_minimap_update);
	if (WritePrivateProfileString("Display_Settings",
		"Fast_Minimap_Update", default_string, file_path) == NULL)
		return false;

	wsprintf(default_string, "%i", hover_sprite_info);
	if (WritePrivateProfileString("General_Settings",
		"Hover_Sprite_Info", default_string, file_path) == NULL)
		return false;

	wsprintf(default_string, "%i", hover_sprite_hardness);
	if (WritePrivateProfileString("General_Settings",
		"Hover_Sprite_Hardness", default_string, file_path) == NULL)
		return false;

	wsprintf(default_string, "%i", help_text);
	if (WritePrivateProfileString("General_Settings",
		"Show_Help_Text", default_string, file_path) == NULL)
		return false;
	
	return true;
}

bool writeDinkIni(char *path)
{
	char windows_path[MAX_PATH];
	char dink_ini_path[MAX_PATH];
	
	// first find the location of the windows directory
	if (GetWindowsDirectory(windows_path, MAX_PATH) == NULL)
		return false;

	wsprintf(dink_ini_path, "%s\\dinksmallwood.ini", windows_path);

	// open the dink.ini file
	ofstream dink_ini(dink_ini_path);

	if (dink_ini == NULL)
		return false;

	dink_ini << "[Dink Smallwood Directory Information for the CD to read]\n" 
		<< path 
		<< "FALSE\n";

	dink_ini.close();

	return true;
}

// gets the path of the main dink directory
bool readDinkIni(char* path)
{
	char windows_path[MAX_PATH];
	char dink_ini_path[MAX_PATH];
	char dink_pathname[MAX_PATH];
	
	// first find the location of the windows directory
	if (GetWindowsDirectory(windows_path, MAX_PATH) == NULL)
		return false;

	wsprintf(dink_ini_path, "%s\\dinksmallwood.ini", windows_path);

	// open the dink.ini file
	ifstream dink_ini(dink_ini_path);

	if (dink_ini == NULL)
		return false;

	// get the first line
	dink_ini.getline(dink_pathname, MAX_PATH);

	if (dink_pathname[0] == 0)
		return false;

	// now get the second line
	dink_ini.getline(dink_pathname, MAX_PATH);

	if (dink_pathname[0] == 0)
		return false;

	//if not slash on the end, we shall add it.
	if (dink_pathname[strlen(dink_pathname)-1] != '\\')
		wsprintf(path, "%s\\", dink_pathname);
	else
		strcpy(path, dink_pathname);

	return true;
}

bool readDmodDiz(char* path, CString &dmod_title, CString &copyright1, 
				 CString &copyright2, CString &description)
{
	char dmod_diz_path[256];

	wsprintf(dmod_diz_path, "%s\\dmod.diz", path);

	// open the dink.diz file
	ifstream dmod_diz(dmod_diz_path);

	if (dmod_diz == NULL)
		return false;

	char buffer[1024];
	
	dmod_diz.getline(buffer, 1024);
	dmod_title = buffer;

	dmod_diz.getline(buffer, 1024);
	copyright1 = buffer;

	dmod_diz.getline(buffer, 1024);
	copyright2 = buffer;

	description = "";
	while (dmod_diz.eof() == false)
	{
		dmod_diz.getline(buffer, 1024);
		description += buffer;
		description += "\n";
	}

	return true;
}

// writes out the dmod.diz file for the dmod
bool writeDmodDiz(char* path, char* dmod_title, char* copyright1, char* copyright2, char* description)
{
	char dmod_diz_path[256];

	wsprintf(dmod_diz_path, "%s\\dmod.diz", path);

	// open the dink.diz file
	ofstream dmod_diz(dmod_diz_path);

	if (dmod_diz == NULL)
		return false;

	dmod_diz << dmod_title << endl;
	dmod_diz << copyright1 << endl;
	dmod_diz << copyright2 << endl;
	dmod_diz << description << endl;

	return true;
}


void CopyAllFiles(char path_in[MAX_PATH], char path_out[MAX_PATH], char sub_path[MAX_PATH])
{
	if (path_in[strlen(path_in) -1] != '\\')
		strcat(path_in, "\\");

	if (path_out[strlen(path_out) -1] != '\\')
		strcat(path_out, "\\");


	//varibles used for the searching
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	bool bMoreFiles = true;

	//strings
	char findpath[250];
	char fullsrcpath[250];
	char fulldestpath[250];
	char in_temp[250];
	char out_temp[250];

	//tell it to find *.*
	sprintf(fullsrcpath, "%s%s*.*", path_in, sub_path);
	hFind = FindFirstFile(fullsrcpath, &dataFind);

	while (hFind != INVALID_HANDLE_VALUE && bMoreFiles == true)
	{
		sprintf(in_temp, "%s%s%s", path_in, sub_path, dataFind.cFileName);
		sprintf(out_temp, "%s%s%s", path_out, sub_path, dataFind.cFileName);

		CopyFile(in_temp, out_temp, TRUE);
		bMoreFiles = FindNextFile(hFind, &dataFind);
	}

	//find new directory
	sprintf(fullsrcpath, "%s%s*.", path_in, sub_path);
	hFind = FindFirstFile(fullsrcpath, &dataFind);
	bMoreFiles = true;

	while (hFind != INVALID_HANDLE_VALUE && bMoreFiles == true)
	{
		//make sure it's directory
		if ((dataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 && dataFind.cFileName[0] != '.')
		{
			char temp[50];

			sprintf(temp, "%s\\", dataFind.cFileName);

			sprintf(in_temp, "%s%s%s", path_in, sub_path, temp);
			sprintf(out_temp, "%s%s%s", path_out, sub_path, temp);
			sprintf(fullsrcpath, "%s%s", path_in, sub_path);
			sprintf(fulldestpath, "%s%s", path_out, sub_path);

			CreateDirectory(out_temp, NULL);
			CopyAllFiles(fullsrcpath, fulldestpath, temp);
		}
		bMoreFiles = FindNextFile(hFind, &dataFind);
	}
}

bool createDmod(char* dmod_dir_name, char* dmod_title, char* copyright1, char* copyright2, char* description)
{
	//varibles needed
	char dmod_path[MAX_PATH];
	char buffer[250];
	char path[MAX_PATH];

	//used for opening files.
	FILE *stream;

	
	wsprintf(dmod_path, "%s%s", main_dink_path.GetBuffer(0), dmod_dir_name);

	// create the main dmod directory
	if (CreateDirectory(dmod_path, NULL) == FALSE)
	{
		char error_message[1024];
		wsprintf(error_message, "Could not create dmod %s, bad dmod name or dmod with that name already exists", dmod_dir_name);
		MessageBox(NULL, error_message, "Could not create dmod", MB_OK);
		return false;
	}

	//use default skeleton location.
	sprintf(buffer, "%s\\skeleton\\HARD.dat", main_dink_path.GetBuffer(0));
	sprintf(path, "%s\\skeleton\\", main_dink_path.GetBuffer(0));

	//try to open the hard.dat (it's the important one)
	while ((stream = fopen(buffer, "rb")) == NULL)
	{
		//propmt user we could not find it.
		MessageBox(mainWnd->GetSafeHwnd(), 
			"Skeleton directory not found. Please select.", NULL, MB_OK);

		//make it look pretty for the user
		sprintf(buffer, "%s\\skeleton", main_dink_path.GetBuffer(0));

		//populate get folder dialog.
		CGetFolderDlg newdlg; //dialog
		newdlg.m_strFolderPath = buffer; //use for the path in edit box
		int retcode = newdlg.DoModal(); //display the box. (draw it)

		//what did the click.
		if (retcode == IDCANCEL)
		{
			//cancel. Notify user d-mod was not created.
			MessageBox(mainWnd->GetSafeHwnd(), "D-mod was not created.", NULL, NULL);
			return false;
		} else {
			//they clicked ok. Lets do it all over again.
			sprintf(buffer, "%s", newdlg.m_strFolderPath.GetBuffer(0));
			sprintf(path, "%s", newdlg.m_strFolderPath.GetBuffer(0));

			if (buffer[strlen(buffer) -1] != '\\')
				sprintf(buffer, "%s\\", buffer);

			sprintf(buffer, "%sHARD.dat", buffer);
		}
	}

	CopyAllFiles(path, dmod_path, "");

	// write out the dmod.diz
	writeDmodDiz(dmod_path, dmod_title, copyright1, copyright2, description);

	return true;
}

void compensateScreenGap(int &x, int &y)
{
	// compensate for screen gap
	x -= SIDEBAR_WIDTH;
	if (x > SCREEN_WIDTH)
	{
		x -= screen_gap;

		if (x < SCREEN_WIDTH)
			x = SCREEN_WIDTH;
	}

	if (x < 0)
	{
		x += screen_gap;

		if (x > 0)
			x = 0;
	}
	x += SIDEBAR_WIDTH;

	if (y > SCREEN_HEIGHT)
	{
		y -= screen_gap;

		if (y < SCREEN_HEIGHT)
			y = SCREEN_HEIGHT;
	}

	if (y < 0)
	{
		y += screen_gap;

		if (y > 0)
			y = 0;
	}
}

bool verifyDirectory(char *directory)
{
	char full_path[256];
	strcpy(full_path, directory);
	strcat(full_path, "\\dink.dat");

	wsprintf(full_path, "%s%s\\dink.dat", main_dink_path.GetBuffer(0), directory);

	FILE* input = fopen(full_path, "rb");

	if (input == NULL)
	{
		return false;
	}
	else
	{
		fclose(input);
		return true;
	}
}

bool createDmodList(char* dink_path, CListBox *dmod_list)
{
	HANDLE hFind;
	WIN32_FIND_DATA dataFind;
	BOOL bMoreFiles = TRUE;
	int i;

	dmod_list->ResetContent();

	char dmod_path[256];
	char filename[MAX_DMOD_NAME_LENGTH];

	wsprintf(dmod_path, "%s*.*", main_dink_path);

	hFind = FindFirstFile(dmod_path, &dataFind);

	// get all the *.c files in the directory
	while(hFind != INVALID_HANDLE_VALUE && bMoreFiles == TRUE)
	{
		if ((dataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0 &&
			dataFind.cFileName[0] != '.')
		{
			strncpy(filename, dataFind.cFileName, MAX_DMOD_NAME_LENGTH);
			filename[MAX_DMOD_NAME_LENGTH - 1] = 0;

			// make lower
			for (i = 1; i < MAX_DMOD_NAME_LENGTH - 1; i++)
			{
				filename[i] = tolower(filename[i]);
			}

			if (verifyDirectory(filename) == true)
				dmod_list->AddString(filename);
		}

		bMoreFiles = FindNextFile(hFind, &dataFind);
	}

	FindClose(hFind);

	return true;
}

int Draw_Text_GDI(char *text, int x, int y, int color, LPDIRECTDRAWSURFACE7 lpdds)
{
	// this function draws the sent text on the sent surface 
	// using color index as the color in the palette

	//if it's out of bounds, kill it.
	if (x < 0)
		x = 0;

	if (y < 0)
		y = 0;

	HDC xdc; // the working dc

	// get the dc from surface
	if (lpdds->GetDC(&xdc) != DD_OK)
	   return false;

	// set the colors for the text up
	SetTextColor(xdc, color);

	// set background mode to transparent so black isn't copied
	SetBkMode(xdc, TRANSPARENT);

	// draw the text a
	TextOut(xdc, x, y, text, strlen(text));

	// release the dc
	lpdds->ReleaseDC(xdc);

	// return success
	return true;
}

///////////////////////////////////////////////////////////

bool fixBounds(RECT &dest_box, RECT &src_box, RECT &clip_box)
{
	if (dest_box.left < clip_box.left)
	{
		src_box.left += clip_box.left - dest_box.left;
		dest_box.left = clip_box.left;
	}

	if (dest_box.right > clip_box.right)
	{
		src_box.right -= dest_box.right - clip_box.right;
		dest_box.right = clip_box.right;
	}

	if (dest_box.left >= dest_box.right)
		return false;

	if (dest_box.top < clip_box.top)
	{
		src_box.top += clip_box.top - dest_box.top;
		dest_box.top = clip_box.top;
	}

	if (dest_box.bottom > clip_box.bottom)
	{
		src_box.bottom -= dest_box.bottom - clip_box.bottom;
		dest_box.bottom = clip_box.bottom;
	}

	if (dest_box.top >= dest_box.bottom)
		return false;

	return true;
}

///////////////////////////////////////////////////////////

bool fixBounds(RECT &box, RECT &clip_box)
{
	int temp; //used to hold for fixing boxes.

	//check if we are inverted, if so, correct.
	if (box.left > box.right)
	{
		temp = box.right;
		box.right = box.left;
		box.left = temp;
	}
	if (box.top > box.bottom)
	{
		temp = box.top;
		box.top = box.bottom;
		box.bottom = temp;
	}
	if (clip_box.left > clip_box.right)
	{
		temp = clip_box.right;
		clip_box.right = clip_box.left;
		clip_box.left = temp;
	}
	if (clip_box.top > clip_box.bottom)
	{
		temp = clip_box.top;
		clip_box.top = clip_box.bottom;
		clip_box.bottom = temp;
	}

	//acctually clip the bounds.
	if (box.left < clip_box.left)
	{
		box.left = clip_box.left;
	}

	if (box.right > clip_box.right)
	{
		box.right = clip_box.right;
	}

	if (box.left >= box.right)
		return false;

	if (box.top < clip_box.top)
	{
		box.top = clip_box.top;
	}

	if (box.bottom > clip_box.bottom)
	{
		box.bottom = clip_box.bottom;
	}

	if (box.top >= box.bottom)
		return false;

	return true;
}

///////////////////////////////////////////////////////////

int drawBox(RECT &box, int color, int line_width)
{
	DDBLTFX	gridcolor;
	memset(&gridcolor,0,sizeof(DDBLTFX));
	gridcolor.dwSize = sizeof(DDBLTFX);
	gridcolor.dwFillColor = color;

	int temp;
	if (box.left > box.right)
	{
		temp = box.left;
		box.left = box.right;
		box.right = temp;
	}

	if (box.top > box.bottom)
	{
		temp = box.top;
		box.top = box.bottom;
		box.bottom = temp;
	}

	// fill in the destination of the box
	RECT dest;
	RECT clip_box = {0, 0, current_map->window_width, current_map->window_height};

	// draw left
	dest.left	= box.left;
	dest.right	= box.left + line_width;
	dest.top	= box.top;
	dest.bottom	= box.bottom;
	if (fixBounds(dest, clip_box))
		lpddsback->Blt(&dest, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &gridcolor);

	// draw right
	dest.left = box.right - line_width;
	dest.right = box.right;
	if (fixBounds(dest, clip_box))
		lpddsback->Blt(&dest, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &gridcolor);
	
	
	// draw top
	dest.left	= box.left + line_width;
	dest.right	= box.right - line_width;
	dest.top	= box.top;
	dest.bottom	= box.top + line_width;
	if (fixBounds(dest, clip_box))
		lpddsback->Blt(&dest, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &gridcolor);
	
	// draw bottom
	dest.top	= box.bottom - line_width;
	dest.bottom	= box.bottom;
	if (fixBounds(dest, clip_box))
		lpddsback->Blt(&dest, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &gridcolor);

	return true;
}

int drawFilledBox(RECT &box, int color, bool transparent, LPDIRECTDRAWSURFACE7 &surface)
{
	DDBLTFX	gridcolor;
	memset(&gridcolor,0,sizeof(DDBLTFX));
	gridcolor.dwSize = sizeof(DDBLTFX);
	gridcolor.dwFillColor = color;

	int temp;
	if (box.left > box.right)
	{
		temp = box.left;
		box.left = box.right;
		box.right = temp;
	}

	if (box.top > box.bottom)
	{
		temp = box.top;
		box.top = box.bottom;
		box.bottom = temp;
	}

	RECT clip_box = {0, 0, current_map->window_width, current_map->window_height};

	fixBounds(box, clip_box);
	
	if (transparent)
	{
		/*RECT box_temp = box;

		temp = box_temp.left;
		box_temp.left = 0;
		box_temp.right -= temp;

		temp = box_temp.top;
		box_temp.top = 0;
		box_temp.bottom -= temp;

		lpddshidden->Blt(&box_temp, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &gridcolor);
		lpddsback->Blt(&box, lpddshidden, &box_temp, DDBLT_WAIT | DDBLT_KEYSRC, NULL);*/
		surface->Blt(&box, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &gridcolor);
	}
	else
	{
		surface->Blt(&box, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &gridcolor);
	}

	return true;
}

void drawLine(int x1, int y1, int x2, int y2, int color)
{
	int slope = 1;

	int temp;
	if (x1 > x2)
	{
		temp = y1;
		y1 = y2;
		y2 = temp;

		temp = x1;
		x1 = x2;
		x2 = temp;
	}

	int width = x2 - x1;
	int height = y2 - y1;

	if (height < 0)
	{
		slope = -1;
		height = -height;
	}

	double ratio = double(width) / double(height);
	double cur_x = double(x1);

	int copy_width;

	int start_x = x1;
	int end_x;
	int i;

	// lock the display surface
	lpddsback->Lock(NULL,
				&ddsdback,
				DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
				NULL);

	// find the dest bitmap pointer

	UCHAR *dest_ptr = (UCHAR *)ddsdback.lpSurface;

	dest_ptr += colorformat * x1 + ddsdback.lPitch * y1;

	for (int cur_y = y1; cur_y != y2; cur_y += slope)
	{
		cur_x += ratio;

		end_x = int(cur_x);

		copy_width = end_x - start_x;

		for (i = 0; i <= copy_width; i++)
		{
			memcpy(dest_ptr, &color, colorformat);
			dest_ptr += colorformat;
		}

		dest_ptr -= colorformat;

		start_x = end_x;

		dest_ptr += ddsdback.lPitch * slope;
	}

	// unlock the surface 
	lpddsback->Unlock(NULL);
}

void ScreenText(char output[MAX_PATH], int y_offset)
{
	if (help_text)
	{
		Draw_Text_GDI(output, 10, current_map->window_height - y_offset, help_text_color, lpddsback);
	}
}