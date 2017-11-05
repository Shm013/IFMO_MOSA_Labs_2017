#define WIN32_NO_STATUS
#define NTOS_MODE_USER
#include <stdio.h>
#include <stdarg.h>
#include <excpt.h>
#include <windef.h>
#include <winnt.h>
#include <ntndk.h>
#include <ntddkbd.h>

// Keyboard:

HANDLE hKeyboard;

typedef struct _KBD_RECORD {
	WORD  wVirtualScanCode;
	DWORD dwControlKeyState;
	UCHAR asciiChar;
	BOOL  bKeyDown;
} KBD_RECORD, *PKBD_RECORD;

// --{{ TODO: make separate handler
void IntTranslateKey(PKEYBOARD_INPUT_DATA InputData, KBD_RECORD *kbd_rec);

#define RIGHT_ALT_PRESSED     0x0001 // the right alt key is pressed.
#define LEFT_ALT_PRESSED      0x0002 // the left alt key is pressed.
#define RIGHT_CTRL_PRESSED    0x0004 // the right ctrl key is pressed.
#define LEFT_CTRL_PRESSED     0x0008 // the left ctrl key is pressed.
#define SHIFT_PRESSED         0x0010 // the shift key is pressed.
#define NUMLOCK_ON            0x0020 // the numlock light is on.
#define SCROLLLOCK_ON         0x0040 // the scrolllock light is on.
#define CAPSLOCK_ON           0x0080 // the capslock light is on.
#define ENHANCED_KEY          0x0100 // the key is enhanced.
// }}--