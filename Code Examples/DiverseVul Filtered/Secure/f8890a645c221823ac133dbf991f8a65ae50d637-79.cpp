static BOOL update_write_bitmap_update(rdpUpdate* update, wStream* s,
                                       const BITMAP_UPDATE* bitmapUpdate)
{
	int i;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	Stream_Write_UINT16(s, UPDATE_TYPE_BITMAP);   /* updateType */
	Stream_Write_UINT16(s, bitmapUpdate->number); /* numberRectangles (2 bytes) */

	/* rectangles */
	for (i = 0; i < (int)bitmapUpdate->number; i++)
	{
		if (!update_write_bitmap_data(update, s, &bitmapUpdate->rectangles[i]))
			return FALSE;
	}

	return TRUE;
}