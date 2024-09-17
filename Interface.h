#ifndef _INTERFACE_H_
#define _INTERFACE_H_

// defines

#define KEY_ESC			27
#define KEY_DELETE		46
#define KEY_CTRL		17
#define KEY_SHIFT		16
#define KEY_TAB			'\t'
#define KEY_ENTER		13
#define KEY_BACKQUOTE	192
#define KEY_PAGEUP		33
#define KEY_PAGEDOWN	34
#define KEY_ARROW_UP	38
#define KEY_ARROW_DOWN	40
#define KEY_ARROW_LEFT	37
#define KEY_ARROW_RIGHT	39

//function key support it SHOULD work, not tested fully.
#define KEY_FUNCTION_1  112
#define KEY_FUNCTION_2  113
#define KEY_FUNCTION_3  114
#define KEY_FUNCTION_4  115
#define KEY_FUNCTION_5  116
#define KEY_FUNCTION_6  117
#define KEY_FUNCTION_7  118
#define KEY_FUNCTION_8  119
#define KEY_FUNCTION_9  120
#define KEY_FUNCTION_10 121
#define KEY_FUNCTION_11 122
#define KEY_FUNCTION_12 123

//these keys should support all plus and minus keys
//including on number pad and keyboard. Language doesn't matter
//#define KEY_PLUS		0x6B | 0xBB
//#define KEY_MINUS		0x6D | 0xBD
//they don't work for some odd reason

// prototypes

class CWinDinkeditView;

void initializeInput();

void keyPressed(UCHAR key);
void keyReleased(UCHAR key);

int mouseLeftPressed(int x, int y);
int mouseLeftReleased(int x, int y);
int mouseRightPressed(int x, int y, CWinDinkeditView* view_window);
int mouseLeftDoubleClick(int x, int y);
int mouseMove(int x, int y);

int setVision(int new_vision);
bool editScript(CString filename);

#endif
