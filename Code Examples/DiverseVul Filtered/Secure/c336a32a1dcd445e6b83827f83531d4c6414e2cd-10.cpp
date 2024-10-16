static void SaveCursor(struct cursor *cursor)
{
	cursor->on = 1;
	cursor->x = curr->w_x;
	cursor->y = curr->w_y;
	cursor->Rend = curr->w_rend;
	cursor->Charset = curr->w_Charset;
	cursor->CharsetR = curr->w_CharsetR;
	memmove((char *)cursor->Charsets, (char *)curr->w_charsets, 4 * sizeof(int));
}