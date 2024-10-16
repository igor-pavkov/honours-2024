static void RestoreCursor(struct cursor *cursor)
{
	if (!cursor->on)
		return;
	LGotoPos(&curr->w_layer, cursor->x, cursor->y);
	curr->w_x = cursor->x;
	curr->w_y = cursor->y;
	curr->w_rend = cursor->Rend;
	memmove((char *)curr->w_charsets, (char *)cursor->Charsets, 4 * sizeof(int));
	curr->w_Charset = cursor->Charset;
	curr->w_CharsetR = cursor->CharsetR;
	curr->w_ss = 0;
	curr->w_FontL = curr->w_charsets[curr->w_Charset];
	curr->w_FontR = curr->w_charsets[curr->w_CharsetR];
	LSetRendition(&curr->w_layer, &curr->w_rend);
}