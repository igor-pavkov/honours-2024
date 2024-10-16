static BOOL update_read_draw_gdiplus_next_order(wStream* s,
                                                DRAW_GDIPLUS_NEXT_ORDER* draw_gdiplus_next)
{
	if (Stream_GetRemainingLength(s) < 3)
		return FALSE;

	Stream_Seek_UINT8(s);                              /* pad1Octet (1 byte) */
	FIELD_SKIP_BUFFER16(s, draw_gdiplus_next->cbSize); /* cbSize(2 bytes) + emfRecords */
	return TRUE;
}