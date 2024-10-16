static void DoESC(int c, int intermediate)
{
	switch (intermediate) {
	case 0:
		switch (c) {
		case 'E':
			LineFeed(1);
			break;
		case 'D':
			LineFeed(0);
			break;
		case 'M':
			ReverseLineFeed();
			break;
		case 'H':
			curr->w_tabs[curr->w_x] = 1;
			break;
		case 'Z':	/* jph: Identify as VT100 */
			Report("\033[?%d;%dc", 1, 2);
			break;
		case '7':
			SaveCursor(&curr->w_saved);
			break;
		case '8':
			RestoreCursor(&curr->w_saved);
			break;
		case 'c':
			ClearScreen();
			ResetWindow(curr);
			LKeypadMode(&curr->w_layer, 0);
			LCursorkeysMode(&curr->w_layer, 0);
#ifndef TIOCPKT
			WNewAutoFlow(curr, 1);
#endif
			/* XXX
			   SetRendition(&mchar_null);
			   InsertMode(0);
			   ChangeScrollRegion(0, rows - 1);
			 */
			LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
			break;
		case '=':
			LKeypadMode(&curr->w_layer, curr->w_keypad = 1);
#ifndef TIOCPKT
			WNewAutoFlow(curr, 0);
#endif				/* !TIOCPKT */
			break;
		case '>':
			LKeypadMode(&curr->w_layer, curr->w_keypad = 0);
#ifndef TIOCPKT
			WNewAutoFlow(curr, 1);
#endif				/* !TIOCPKT */
			break;
		case 'n':	/* LS2 */
			MapCharset(G2);
			break;
		case 'o':	/* LS3 */
			MapCharset(G3);
			break;
		case '~':
			MapCharsetR(G1);	/* LS1R */
			break;
			/* { */
		case '}':
			MapCharsetR(G2);	/* LS2R */
			break;
		case '|':
			MapCharsetR(G3);	/* LS3R */
			break;
		case 'N':	/* SS2 */
			if (curr->w_charsets[curr->w_Charset] != curr->w_charsets[G2]
			    || curr->w_charsets[curr->w_CharsetR] != curr->w_charsets[G2])
				curr->w_FontR = curr->w_FontL = curr->w_charsets[curr->w_ss = G2];
			else
				curr->w_ss = 0;
			break;
		case 'O':	/* SS3 */
			if (curr->w_charsets[curr->w_Charset] != curr->w_charsets[G3]
			    || curr->w_charsets[curr->w_CharsetR] != curr->w_charsets[G3])
				curr->w_FontR = curr->w_FontL = curr->w_charsets[curr->w_ss = G3];
			else
				curr->w_ss = 0;
			break;
		case 'g':	/* VBELL, private screen sequence */
			WBell(curr, 1);
			break;
		}
		break;
	case '#':
		switch (c) {
		case '8':
			FillWithEs();
			break;
		}
		break;
	case '(':
		DesignateCharset(c, G0);
		break;
	case ')':
		DesignateCharset(c, G1);
		break;
	case '*':
		DesignateCharset(c, G2);
		break;
	case '+':
		DesignateCharset(c, G3);
		break;
/*
 * ESC $ ( Fn: invoke multi-byte charset, Fn, to G0
 * ESC $ Fn: same as above.  (old sequence)
 * ESC $ ) Fn: invoke multi-byte charset, Fn, to G1
 * ESC $ * Fn: invoke multi-byte charset, Fn, to G2
 * ESC $ + Fn: invoke multi-byte charset, Fn, to G3
 */
	case '$':
	case '$' << 8 | '(':
		DesignateCharset(c & 037, G0);
		break;
	case '$' << 8 | ')':
		DesignateCharset(c & 037, G1);
		break;
	case '$' << 8 | '*':
		DesignateCharset(c & 037, G2);
		break;
	case '$' << 8 | '+':
		DesignateCharset(c & 037, G3);
		break;
	}
}