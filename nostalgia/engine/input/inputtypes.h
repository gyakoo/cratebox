#ifndef _GYINPUTTYPES_H_
#define _GYINPUTTYPES_H_

enum gyInputDevice
{
  GY_INPUTDEV_KEYBOARD,
  GY_INPUTDEV_MOUSE,
  GY_INPUTDEV_GAMEPAD_0,
  GY_INPUTDEV_GAMEPAD_1,
  GY_INPUTDEV_GAMEPAD_2,
  GY_INPUTDEV_GAMEPAD_3,
  GY_INPUTDEV_MAXGAMEPADS
};

enum gyInputIndex
{
  //////////////////////////////////////////////////////////////////////////
  // GENERIC PAD
  //////////////////////////////////////////////////////////////////////////
  GYII_PAD_FIRST_ELEMENT,
  GYII_PAD_START=GYII_PAD_FIRST_ELEMENT,
  GYII_PAD_BACK_PAUSE,

  GYII_PAD_LPAD_DOWN,
  GYII_PAD_LPAD_UP,
  GYII_PAD_LPAD_LEFT,
  GYII_PAD_LPAD_RIGHT,

  GYII_PAD_RPAD_DOWN,
  GYII_PAD_RPAD_UP,
  GYII_PAD_RPAD_LEFT,
  GYII_PAD_RPAD_RIGHT,
  
  GYII_PAD_LSTICK_BUTTON,
  GYII_PAD_RSTICK_BUTTON,
  
  GYII_PAD_LSHOULDER,
  GYII_PAD_RSHOULDER,
  GYII_PAD_LTRIGGER,
  GYII_PAD_RTRIGGER,
  
  GYII_PAD_LSTICK_X,
  GYII_PAD_LSTICK_Y,
  
  GYII_PAD_RSTICK_X,
  GYII_PAD_RSTICK_Y,

  GYII_PAD_UNPLUGGED,
  GYII_PAD_PLUGGED,
  GYII_PAD_LAST_ELEMENT=GYII_PAD_PLUGGED,

  //////////////////////////////////////////////////////////////////////////
  // MOUSE
  //////////////////////////////////////////////////////////////////////////
  GYII_MO_FIRST_ELEMENT,
  GYII_MO_LEFT_BUTTON_DOWN=GYII_MO_FIRST_ELEMENT,
  GYII_MO_RIGHT_BUTTON_DOWN,
  GYII_MO_MIDDLE_BUTTON_DOWN,
  GYII_MO_LEFT_BUTTON_UP,
  GYII_MO_RIGHT_BUTTON_UP,
  GYII_MO_MIDDLE_BUTTON_UP,
  GYII_MO_WHEEL_UP,
  GYII_MO_WHEEL_DOWN,
  GYII_MO_LEFT_DBLCLICK,
  GYII_MO_RIGHT_DBLCLICK,
  GYII_MO_MIDDLE_DBLCLICK,
  GYII_MO_LAST_ELEMENT=GYII_MO_MIDDLE_DBLCLICK,

  //////////////////////////////////////////////////////////////////////////
  // KEYBOARD
  //////////////////////////////////////////////////////////////////////////
  GYII_KB_UNKNOWN, ///< Unknown key
  GYII_KB_ANYKEY = GYII_KB_UNKNOWN, ///< can be passed to test any key between GYII_KB_FIRST_ELEMENT and GYII_KB_LAST_ELEMENT)

  GYII_KB_FIRST_ELEMENT = GYII_KB_UNKNOWN + 2, ///< first keyboard relevant element (there is also a GYII_KB_LAST_ELEMENT)

  // Control keys
  GYII_KB_LSHIFT = GYII_KB_FIRST_ELEMENT,  ///< Left shift (or shift, if no right shift exists)
  GYII_KB_RSHIFT,  ///< Right shift
  GYII_KB_LCTRL,   ///< Left control (or control, if no right control exists)
  GYII_KB_RCTRL,   ///< Right control

  GYII_KB_LALT,    ///< Left alt (or alt, if no right alt exists)
  GYII_KB_RALT,    ///< Right Alt / AltGr (Right Alt on German keyboards)
  GYII_KB_ALTGR = GYII_KB_RALT,

  GYII_KB_BACKSP,  ///< Backspace (ASCII compliant)
  GYII_KB_TAB,     ///< Tab (ASCII compliant)

  GYII_KB_LWIN,    ///< Left Windows key
  GYII_KB_RWIN,    ///< Right Windows key
  GYII_KB_APPS,    ///< Windows menu (application) key  //Used to be called VGLK_MENU incorrectly
  GYII_KB_ENTER,   ///< Guess what :) (ASCII compliant)
  GYII_KB_LCOM,    ///< Left command (Mac)
  GYII_KB_RCOM,    ///< Right command (Mac)
  GYII_KB_MAC,     ///< Special mac key (will come later :)
  GYII_KB_LMETA,   ///< Left meta key
  GYII_KB_RMETA,   ///< Right meta key

  // Non-numerical edit and cursor movement keys
  GYII_KB_INS = GYII_KB_RMETA +3,   ///< Insert
  GYII_KB_DEL,   ///< Delete
  GYII_KB_HOME,  ///< Home
  GYII_KB_END,   ///< End
  GYII_KB_PGUP,  ///< PageUp
  GYII_KB_PGDN,  ///< PageDown

  GYII_KB_ESC,   ///< Escape (ASCII compliant)

  GYII_KB_UP,    ///< Up key
  GYII_KB_DOWN,  ///< Down key
  GYII_KB_LEFT,  ///< Left key
  GYII_KB_RIGHT, ///< Right key

  GYII_KB_SPACE, ///< Space bar (ASCII compliant)

  //The following keys might not be available on all keyboard layouts:
  // (They work on US and German layouts)
  GYII_KB_COMMA,  ///< Comma "," on american keyboard
  GYII_KB_PERIOD, ///< Period "." on american keyboard
  GYII_KB_MINUS,  ///< Minus on american keyboard

  GYII_KB_GRAVE,  ///< Grave ` on american keyboard (same key as tilde)
  GYII_KB_CONSOLE_ENABLE_KEY1 = GYII_KB_GRAVE,

  // Standard number codes comply with ASCII codes
  GYII_KB_0 = GYII_KB_GRAVE +12,
  GYII_KB_1,
  GYII_KB_2,
  GYII_KB_3,
  GYII_KB_4,
  GYII_KB_5,
  GYII_KB_6,
  GYII_KB_7,
  GYII_KB_8,
  GYII_KB_9,

  // Special keys
  GYII_KB_CAPS,    ///< Caps Lock
  GYII_KB_SCROLL,  ///< Scroll Lock
  GYII_KB_NUM,     ///< Num Lock
  GYII_KB_PRSCR,   ///< Print screen
  GYII_KB_PAUSE,   ///< Pause key

  // Standard letter codes comply with ASCII codes
  GYII_KB_A = GYII_KB_PAUSE + 3,
  GYII_KB_B,
  GYII_KB_C,
  GYII_KB_D,
  GYII_KB_E,
  GYII_KB_F,
  GYII_KB_G,
  GYII_KB_H,
  GYII_KB_I,
  GYII_KB_J,
  GYII_KB_K,
  GYII_KB_L,
  GYII_KB_M,
  GYII_KB_N,
  GYII_KB_O,
  GYII_KB_P,
  GYII_KB_Q,
  GYII_KB_R,
  GYII_KB_S,
  GYII_KB_T,
  GYII_KB_U,
  GYII_KB_V,
  GYII_KB_W,
  GYII_KB_X,
  GYII_KB_Y,
  GYII_KB_Z,

  // Numeric keypad keys. Some of these keys are doubled for your leisure :)
  GYII_KB_KP_SLASH,  ///< Slash (or divide) key on the numeric keypad
  GYII_KB_KP_MUL,    ///< Multiply (or asterisk) key on the numeric keypad
  GYII_KB_KP_MINUS,  ///< Minus on the numeric keypad
  GYII_KB_KP_PLUS,   ///< Plus...
  GYII_KB_KP_ENTER, 
  GYII_KB_KP_PERIOD, ///< Period (or comma or del) on the numeric keypad
  GYII_KB_KP_DEL = GYII_KB_KP_PERIOD,
  GYII_KB_KP_0,      ///< Insert or 0
  GYII_KB_KP_INS = GYII_KB_KP_0,
  GYII_KB_KP_1,      ///< End or 1
  GYII_KB_KP_END = GYII_KB_KP_1,
  GYII_KB_KP_2,      ///< Down or 2
  GYII_KB_KP_DOWN = GYII_KB_KP_2,
  GYII_KB_KP_3,      ///< PgDn or 3
  GYII_KP_KP_PGDN = GYII_KB_KP_3,
  GYII_KB_KP_4,      ///< Left or 4
  GYII_KB_KP_LEFT = GYII_KB_KP_4,
  GYII_KB_KP_5,      ///< Keypad 5
  GYII_KB_KP_6,      ///< Right or 6
  GYII_KB_KP_RIGHT = GYII_KB_KP_6,
  GYII_KB_KP_7,      ///< Home or 7
  GYII_KB_KP_HOME = GYII_KB_KP_7,
  GYII_KB_KP_8,      ///< Up or 8
  GYII_KB_KP_UP = GYII_KB_KP_8,
  GYII_KB_KP_9,      ///< PgUp or 9
  GYII_KP_KP_PGUP = GYII_KB_KP_9,

  GYII_KB_F1 = GYII_KP_KP_PGUP + 4,
  GYII_KB_F2,
  GYII_KB_F3,
  GYII_KB_F4,
  GYII_KB_F5,
  GYII_KB_F6,
  GYII_KB_F7,
  GYII_KB_F8,
  GYII_KB_F9,
  GYII_KB_F10,
  GYII_KB_F11,
  GYII_KB_F12,

  GYII_KB_EQUAL = GYII_KB_F12 + 9,  ///< Equal "=" on american keyboard
  GYII_KB_EQUAL_LSQBRK,         ///< Left square bracket "[" on american keyboard
  GYII_KB_EQUAL_RSQBRK,         ///< Right square bracket "]" on american keyboard
  GYII_KB_EQUAL_SEMICL,         ///< Semicolon ";" on american keyboard
  GYII_KB_EQUAL_APOSTR,         ///< Apostrophe key "'" on american keyboard
  GYII_KB_EQUAL_BACKSL,         ///< Backslash "\" on american keyboard
  GYII_KB_EQUAL_SLASH,          ///< Slash "/" on american keyboard

  GYII_KB_DE_SS = GYII_KB_EQUAL_SLASH + 4, /// Double-S (or Beta) on german keyboard
  GYII_KB_DE_ACCENT,  ///< Accent key on german keyboard
  GYII_KB_DE_UE,      ///< U with umlaut on german keyboard
  GYII_KB_DE_PLUS,    ///< Plus on german keyboard
  GYII_KB_DE_OE,      ///< O with umlaut on german keyboard
  GYII_KB_DE_AE,      ///< A with umlaut on german keyboard
  GYII_KB_DE_HASH,    ///< Hash "#" on german keyboard
  GYII_KB_DE_LT,      ///< Less than "<" key on german keyboard
  GYII_KB_DE_CIRC,    ///< Circumflex "^"on german keyboard

  GYII_KB_CONSOLE_ENABLE_KEY2 = GYII_KB_DE_CIRC,

  GYII_KB_LAST_ELEMENT = GYII_KB_DE_CIRC,
};

#endif