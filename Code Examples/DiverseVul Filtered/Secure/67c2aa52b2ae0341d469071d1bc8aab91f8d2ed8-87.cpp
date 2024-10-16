static BOOL update_read_stream_bitmap_first_order(wStream* s,
                                                  STREAM_BITMAP_FIRST_ORDER* stream_bitmap_first)
{
	if (Stream_GetRemainingLength(s) < 10) // 8 + 2 at least
		return FALSE;

	Stream_Read_UINT8(s, stream_bitmap_first->bitmapFlags); /* bitmapFlags (1 byte) */
	Stream_Read_UINT8(s, stream_bitmap_first->bitmapBpp);   /* bitmapBpp (1 byte) */

	if ((stream_bitmap_first->bitmapBpp < 1) || (stream_bitmap_first->bitmapBpp > 32))
	{
		WLog_ERR(TAG, "invalid bpp value %" PRIu32 "", stream_bitmap_first->bitmapBpp);
		return FALSE;
	}

	Stream_Read_UINT16(s, stream_bitmap_first->bitmapType);   /* bitmapType (2 bytes) */
	Stream_Read_UINT16(s, stream_bitmap_first->bitmapWidth);  /* bitmapWidth (2 bytes) */
	Stream_Read_UINT16(s, stream_bitmap_first->bitmapHeight); /* bitmapHeigth (2 bytes) */

	if (stream_bitmap_first->bitmapFlags & STREAM_BITMAP_V2)
	{
		if (Stream_GetRemainingLength(s) < 4)
			return FALSE;

		Stream_Read_UINT32(s, stream_bitmap_first->bitmapSize); /* bitmapSize (4 bytes) */
	}
	else
	{
		if (Stream_GetRemainingLength(s) < 2)
			return FALSE;

		Stream_Read_UINT16(s, stream_bitmap_first->bitmapSize); /* bitmapSize (2 bytes) */
	}

	FIELD_SKIP_BUFFER16(
	    s, stream_bitmap_first->bitmapBlockSize); /* bitmapBlockSize(2 bytes) + bitmapBlock */
	return TRUE;
}