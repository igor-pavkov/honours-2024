static BOOL update_read_draw_gdiplus_first_order(wStream* s,
                                                 DRAW_GDIPLUS_FIRST_ORDER* draw_gdiplus_first)
{
	if (Stream_GetRemainingLength(s) < 11)
		return FALSE;

	Stream_Seek_UINT8(s);                                      /* pad1Octet (1 byte) */
	Stream_Read_UINT16(s, draw_gdiplus_first->cbSize);         /* cbSize (2 bytes) */
	Stream_Read_UINT32(s, draw_gdiplus_first->cbTotalSize);    /* cbTotalSize (4 bytes) */
	Stream_Read_UINT32(s, draw_gdiplus_first->cbTotalEmfSize); /* cbTotalEmfSize (4 bytes) */
	return Stream_SafeSeek(s, draw_gdiplus_first->cbSize);     /* emfRecords */
}