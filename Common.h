#ifndef _COMMON_H_
#define _COMMON_H_

#include <ddraw.h>

// PROTOTYPES /////////////////////////////////////////////

int Draw_Text_GDI(char *text, int x, int y, int color, LPDIRECTDRAWSURFACE7 lpdds);

bool readDinkIni(char* path);
bool writeDinkIni(char *path);

bool readWDEIni();
bool writeWDEIni();
bool readDmodDiz(char* path, CString &dmod_title, CString &copyright1, CString &copyright2, CString &description);
bool writeDmodDiz(char* path, char* dmod_title, char* copyright1, char* copyright2, char* description);

bool createDmod(char* dmod_dir_name, char* dmod_title, char* copyright1, char* copyright2, char* description);
CString getDmodName(CString dmod_path);
bool createDmodList(char* dink_path, CListBox *dmod_list);
void compensateScreenGap(int &x, int &y);
void CopyAllFiles(char* path_in, char* path_out, char* sub_path);

bool fixBounds(RECT &dest_box, RECT &src_box, RECT &clip_box);
bool fixBounds(RECT &box, RECT &clip_box);
int drawBox(RECT &box, int color, int line_width);
int drawFilledBox(RECT &box, int color, bool transparent, LPDIRECTDRAWSURFACE7 &surface);
void drawLine(int x1, int y1, int x2, int y2, int color);
void ScreenText(char output[MAX_PATH], int y_offset);

#endif