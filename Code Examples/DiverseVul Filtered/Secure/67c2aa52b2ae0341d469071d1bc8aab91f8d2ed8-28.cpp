static BOOL update_read_stream_bitmap_next_order(wStream* s,
                                                 STREAM_BITMAP_NEXT_ORDER* stream_bitmap_next)
{
	if (Stream_GetRemainingLength(s) < 5)
		return FALSE;

	Stream_Read_UINT8(s, stream_bitmap_next->bitmapFlags); /* bitmapFlags (1 byte) */
	Stream_Read_UINT16(s, stream_bitmap_next->bitmapType); /* bitmapType (2 bytes) */
	FIELD_SKIP_BUFFER16(
	    s, stream_bitmap_next->bitmapBlockSize); /* bitmapBlockSize(2 bytes) + bitmapBlock */
	return TRUE;
}