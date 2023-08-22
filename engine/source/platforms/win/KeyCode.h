#pragma once

namespace Input
{
    enum KeyCode
{
    //
    // 摘要:
    //     Not assigned (never returned as the result of a keystroke).
    None = 0,
    
    //
    // 摘要:
    //     Mouse
    LMouse = 0x01,
    RMouse = 0x02,
    Cancel = 0x03,
    MidMouse = 0x04,

    Backspace = 0x08,   //     The backspace key.
    Tab = 0x09,         //     The tab key.
    Clear =	 0x0C,	    //      CLEAR key
    Return = 0x0D,	    //      ENTER key

    Shift = 0x10,       //     The shift key.
    Control = 0x11,     //     The ctrl (control) key.
    Menu = 0x12,        //     The menu key.
    Pause = 0x13,       //     The pause key.
    CapsLock = 0x14,    //     The caps lock key.

    Escape = 0x1B,      //     The escape key.
    Space = 0x20,       //     The space bar.
    PageUp = 0x21,      //     Page Up key
    PageDown = 0x22,    //     Page Down key
    End = 0x23,         //     End key
    Home = 0x24,        //     Home key
    LeftArrow = 0x25,   //     Left Arrow key
    UpArrow = 0x26,     //     Up Arrow key
    RightArrow = 0x27,  //     Right Arrow key
    DownArrow = 0x28,   //     Down Arrow key
    
    Select = 0x29,      //     Select key
    Print = 0x2A,       //     Print key
    Execute = 0x2B,     //     Execute key
    PrintScreen = 0x2C, //     Print Screen key
    Insert = 0x2D,      //     Insert key
    Delete = 0x2E,      //     Delete key
    Help = 0x2F,        //     Help key

    Key0 = 0x30,        //     0 key
    Key1 = 0x31,        //     1 key
    Key2 = 0x32,        //     2 key
    Key3 = 0x33,        //     3 key
    Key4 = 0x34,        //     4 key
    Key5 = 0x35,        //     5 key
    Key6 = 0x36,        //     6 key
    Key7 = 0x37,        //     7 key
    Key8 = 0x38,        //     8 key
    Key9 = 0x39,        //     9 key

    A = 0x41,           //     A key
    B = 0x42,           //     B key
    C = 0x43,           //     C key
    D = 0x44,           //     D key
    E = 0x45,           //     E key
    F = 0x46,           //     F key
    G = 0x47,           //     G key
    H = 0x48,           //     H key
    I = 0x49,           //     I key
    J = 0x4A,           //     J key
    K = 0x4B,           //     K key
    L = 0x4C,           //     L key
    M = 0x4D,           //     M key
    N = 0x4E,           //     N key
    O = 0x4F,           //     O key
    P = 0x50,           //     P key
    Q = 0x51,           //     Q key
    R = 0x52,           //     R key
    S = 0x53,           //     S key
    T = 0x54,           //     T key
    U = 0x55,           //     U key
    V = 0x56,           //     V key
    W = 0x57,           //     W key
    X = 0x58,           //     X key
    Y = 0x59,           //     Y key
    Z = 0x5A,           //     Z key
    
    LeftWindows = 0x5B, //     Left Windows key (Natural keyboard)
    RightWindows = 0x5C,//     Right Windows key (Natural keyboard)
    Applications = 0x5D,//     Applications key (Natural keyboard)
    Sleep = 0x5F,       //     Computer Sleep key

    NumPad0 = 0x60,     //     Numeric keypad 0 key
    NumPad1 = 0x61,     //     Numeric keypad 1 key
    NumPad2 = 0x62,     //     Numeric keypad 2 key
    NumPad3 = 0x63,     //     Numeric keypad 3 key
    NumPad4 = 0x64,     //     Numeric keypad 4 key
    NumPad5 = 0x65,     //     Numeric keypad 5 key
    NumPad6 = 0x66,     //     Numeric keypad 6 key
    NumPad7 = 0x67,     //     Numeric keypad 7 key
    NumPad8 = 0x68,     //     Numeric keypad 8 key
    NumPad9 = 0x69,     //     Numeric keypad 9 key

    Multiply = 0x6A,    //     Multiply key
    Add = 0x6B,         //     Add key
    Separator = 0x6C,   //     Separator key
    Subtract = 0x6D,    //     Subtract key
    Decimal = 0x6E,     //     Decimal key
    Divide = 0x6F,      //     Divide key
    
    F1 = 0x70,          //     F1 key
    F2 = 0x71,          //     F2 key
    F3 = 0x72,          //     F3 key
    F4 = 0x73,          //     F4 key
    F5 = 0x74,          //     F5 key
    F6 = 0x75,          //     F6 key
    F7 = 0x76,          //     F7 key
    F8 = 0x77,          //     F8 key
    F9 = 0x78,          //     F9 key
    F10 = 0x79,         //     F10 key
    F11 = 0x7A,         //     F11 key
    F12 = 0x7B,         //     F12 key
    F13 = 0x7C,         //     F13 key
    F14 = 0x7D,         //     F14 key
    F15 = 0x7E,         //     F15 key
    F16 = 0x7F,         //     F16 key
    F17 = 0x80,         //     F17 key
    F18 = 0x81,         //     F18 key
    F19 = 0x82,         //     F19 key
    F20 = 0x83,         //     F20 key
    F21 = 0x84,         //     F21 key
    F22 = 0x85,         //     F22 key
    F23 = 0x86,         //     F23 key
    F24 = 0x87,         //     F24 key
    
    NumLock = 0x90,     //     NUM LOCK key
    ScrollLock = 0x91,  //     SCROLL LOCK key

    LeftShift = 0xA0,   //     Left SHIFT key
    RightShift = 0xA1,  //     Right SHIFT key
    LeftControl = 0xA2, //     Left CONTROL key
    RightControl = 0xA3,//     Right CONTROL key
    LeftMenu = 0xA4,    //     Left MENU key
    RightMenu = 0xA5,   //     Right MENU key

    BrowserBack = 0xA6, //     Browser Back key
    BrowserForward = 0xA7,//    Browser Forward key
    BrowserRefresh = 0xA8,//    Browser Refresh key
    BrowserStop = 0xA9, //     Browser Stop key
    BrowserSearch = 0xAA,//     Browser Search key
    BrowserFavorites = 0xAB,//  Browser Favorites key
    BrowserHome = 0xAC, //     Browser Start and Home key

    VolumeMute = 0xAD,  //     Volume Mute key
    VolumeDown = 0xAE,  //     Volume Down key
    VolumeUp = 0xAF,    //     Volume Up key

    MediaNextTrack = 0xB0,//    Next Track key
    MediaPrevTrack = 0xB1,//    Previous Track key
    MediaStop = 0xB2,   //     Stop Media key
    MediaPlayPause = 0xB3,//    Play/Pause Media key

    LaunchMail = 0xB4,  //     Start Mail key
    LaunchMediaSelect = 0xB5,//  Select Media key


    Semicolon = 0xBA,   //     For the US standard keyboard, the ';:' key
    Plus = 0xBB,        //     For any country/region, the '+' key
    Comma = 0xBC,       //     For any country/region, the ',' key
    Minus = 0xBD,       //     For any country/region, the '-' key
    Period = 0xBE,      //     For any country/region, the '.' key
    Slash = 0xBF,       //     For the US standard keyboard, the '/?' key
    Tilde = 0xC0,       //     For the US standard keyboard, the '`~' key
    
    LeftBracket = 0xDB, //     For the US standard keyboard, the '[{' key
    Backslash = 0xDC,   //     For the US standard keyboard, the '\|' key
    RightBracket = 0xDD,//     For the US standard keyboard, the ']}' key
    Quote = 0xDE,       //     For the US standard keyboard, the 'single-quote/double-quote' key

    OEM8 = 0xDF,        //     Either the angle bracket key or the backslash key on the RT 102-key keyboard
    OEM102 = 0xE2,      //     Either the angle bracket key or the backslash key on the RT 102-key keyboard

    ProcessKey = 0xE5,  //     IME PROCESS key

    Packet = 0xE7,      //     Used to pass Unicode characters as if they were keystrokes.

    Attn = 0xF6,        //     Attn key
    Crsel = 0xF7,       //     CrSel key
    Exsel = 0xF8,       //     ExSel key
    EraseEof = 0xF9,    //     Erase EOF key
    Play = 0xFA,        //     Play key
    Zoom = 0xFB,        //     Zoom key
    NoName = 0xFC,      //     Reserved
    Pa1 = 0xFD,         //     PA1 key
    OEMClear = 0xFE,    //     Clear key

};
}