static UChar32 linenoiseReadChar(void) {
#ifdef _WIN32

    INPUT_RECORD rec;
    DWORD count;
    int modifierKeys = 0;
    bool escSeen = false;
    while (true) {
        ReadConsoleInputW(console_in, &rec, 1, &count);
#if 0  // helper for debugging keystrokes, display info in the debug "Output" window in the debugger
        {
            if ( rec.EventType == KEY_EVENT ) {
                //if ( rec.Event.KeyEvent.uChar.UnicodeChar ) {
                    char buf[1024];
                    sprintf(
                            buf,
                            "Unicode character 0x%04X, repeat count %d, virtual keycode 0x%04X, "
                            "virtual scancode 0x%04X, key %s%s%s%s%s\n",
                            rec.Event.KeyEvent.uChar.UnicodeChar,
                            rec.Event.KeyEvent.wRepeatCount,
                            rec.Event.KeyEvent.wVirtualKeyCode,
                            rec.Event.KeyEvent.wVirtualScanCode,
                            rec.Event.KeyEvent.bKeyDown ? "down" : "up",
                                (rec.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED)  ?
                                    " L-Ctrl" : "",
                                (rec.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) ?
                                    " R-Ctrl" : "",
                                (rec.Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED)   ?
                                    " L-Alt"  : "",
                                (rec.Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED)  ?
                                    " R-Alt"  : ""
                           );
                    OutputDebugStringA( buf );
                //}
            }
        }
#endif
        if (rec.EventType != KEY_EVENT) {
            continue;
        }
        // Windows provides for entry of characters that are not on your keyboard by sending the
        // Unicode characters as a "key up" with virtual keycode 0x12 (VK_MENU == Alt key) ...
        // accept these characters, otherwise only process characters on "key down"
        if (!rec.Event.KeyEvent.bKeyDown && rec.Event.KeyEvent.wVirtualKeyCode != VK_MENU) {
            continue;
        }
        modifierKeys = 0;
        // AltGr is encoded as ( LEFT_CTRL_PRESSED | RIGHT_ALT_PRESSED ), so don't treat this
        // combination as either CTRL or META we just turn off those two bits, so it is still
        // possible to combine CTRL and/or META with an AltGr key by using right-Ctrl and/or
        // left-Alt
        if ((rec.Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_ALT_PRESSED)) ==
            (LEFT_CTRL_PRESSED | RIGHT_ALT_PRESSED)) {
            rec.Event.KeyEvent.dwControlKeyState &= ~(LEFT_CTRL_PRESSED | RIGHT_ALT_PRESSED);
        }
        if (rec.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) {
            modifierKeys |= CTRL;
        }
        if (rec.Event.KeyEvent.dwControlKeyState & (RIGHT_ALT_PRESSED | LEFT_ALT_PRESSED)) {
            modifierKeys |= META;
        }
        if (escSeen) {
            modifierKeys |= META;
        }
        if (rec.Event.KeyEvent.uChar.UnicodeChar == 0) {
            switch (rec.Event.KeyEvent.wVirtualKeyCode) {
                case VK_LEFT:
                    return modifierKeys | LEFT_ARROW_KEY;
                case VK_RIGHT:
                    return modifierKeys | RIGHT_ARROW_KEY;
                case VK_UP:
                    return modifierKeys | UP_ARROW_KEY;
                case VK_DOWN:
                    return modifierKeys | DOWN_ARROW_KEY;
                case VK_DELETE:
                    return modifierKeys | DELETE_KEY;
                case VK_HOME:
                    return modifierKeys | HOME_KEY;
                case VK_END:
                    return modifierKeys | END_KEY;
                case VK_PRIOR:
                    return modifierKeys | PAGE_UP_KEY;
                case VK_NEXT:
                    return modifierKeys | PAGE_DOWN_KEY;
                default:
                    continue;  // in raw mode, ReadConsoleInput shows shift, ctrl ...
            }                  //  ... ignore them
        } else if (rec.Event.KeyEvent.uChar.UnicodeChar ==
                   ctrlChar('[')) {  // ESC, set flag for later
            escSeen = true;
            continue;
        } else {
            // we got a real character, return it
            return modifierKeys | rec.Event.KeyEvent.uChar.UnicodeChar;
        }
    }

#else
    UChar32 c;
    c = readUnicodeCharacter();
    if (c == 0)
        return 0;

// If _DEBUG_LINUX_KEYBOARD is set, then ctrl-^ puts us into a keyboard debugging mode
// where we print out decimal and decoded values for whatever the "terminal" program
// gives us on different keystrokes.  Hit ctrl-C to exit this mode.
//
#define _DEBUG_LINUX_KEYBOARD
#if defined(_DEBUG_LINUX_KEYBOARD)
    if (c == ctrlChar('^')) {  // ctrl-^, special debug mode, prints all keys hit, ctrl-C to get out
        printf("\nEntering keyboard debugging mode (on ctrl-^), press ctrl-C to exit this mode\n");
        while (true) {
            unsigned char keys[10];
            int ret = read(0, keys, 10);

            if (ret <= 0) {
                printf("\nret: %d\n", ret);
            }
            for (int i = 0; i < ret; ++i) {
                UChar32 key = static_cast<UChar32>(keys[i]);
                char* friendlyTextPtr;
                char friendlyTextBuf[10];
                const char* prefixText = (key < 0x80) ? "" : "0x80+";
                UChar32 keyCopy = (key < 0x80) ? key : key - 0x80;
                if (keyCopy >= '!' && keyCopy <= '~') {  // printable
                    friendlyTextBuf[0] = '\'';
                    friendlyTextBuf[1] = keyCopy;
                    friendlyTextBuf[2] = '\'';
                    friendlyTextBuf[3] = 0;
                    friendlyTextPtr = friendlyTextBuf;
                } else if (keyCopy == ' ') {
                    friendlyTextPtr = const_cast<char*>("space");
                } else if (keyCopy == 27) {
                    friendlyTextPtr = const_cast<char*>("ESC");
                } else if (keyCopy == 0) {
                    friendlyTextPtr = const_cast<char*>("NUL");
                } else if (keyCopy == 127) {
                    friendlyTextPtr = const_cast<char*>("DEL");
                } else {
                    friendlyTextBuf[0] = '^';
                    friendlyTextBuf[1] = keyCopy + 0x40;
                    friendlyTextBuf[2] = 0;
                    friendlyTextPtr = friendlyTextBuf;
                }
                printf("%d x%02X (%s%s)  ", key, key, prefixText, friendlyTextPtr);
            }
            printf("\x1b[1G\n");  // go to first column of new line

            // drop out of this loop on ctrl-C
            if (keys[0] == ctrlChar('C')) {
                printf("Leaving keyboard debugging mode (on ctrl-C)\n");
                fflush(stdout);
                return -2;
            }
        }
    }
#endif  // _DEBUG_LINUX_KEYBOARD

    EscapeSequenceProcessing::thisKeyMetaCtrl = 0;  // no modifiers yet at initialDispatch
    return EscapeSequenceProcessing::doDispatch(c, EscapeSequenceProcessing::initialDispatch);
#endif  // #_WIN32
}