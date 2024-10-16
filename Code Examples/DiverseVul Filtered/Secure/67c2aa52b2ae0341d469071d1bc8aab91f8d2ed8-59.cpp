static BOOL update_read_draw_gdiplus_end_order(wStream* s, DRAW_GDIPLUS_END_ORDER* draw_gdiplus_end)
{
	if (Stream_GetRemainingLength(s) < 11)
		return FALSE;

	Stream_Seek_UINT8(s);                                    /* pad1Octet (1 byte) */
	Stream_Read_UINT16(s, draw_gdiplus_end->cbSize);         /* cbSize (2 bytes) */
	Stream_Read_UINT32(s, draw_gdiplus_end->cbTotalSize);    /* cbTotalSize (4 bytes) */
	Stream_Read_UINT32(s, draw_gdiplus_end->cbTotalEmfSize); /* cbTotalEmfSize (4 bytes) */
	return Stream_SafeSeek(s, draw_gdiplus_end->cbSize);     /* emfRecords */
}