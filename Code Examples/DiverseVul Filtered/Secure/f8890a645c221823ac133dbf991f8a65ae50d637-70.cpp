static BOOL update_send_cache_bitmap(rdpContext* context, const CACHE_BITMAP_ORDER* cache_bitmap)
{
	wStream* s;
	size_t bm, em;
	BYTE orderType;
	int headerLength;
	int inf;
	UINT16 extraFlags;
	INT16 orderLength;
	rdpUpdate* update = context->update;
	extraFlags = 0;
	headerLength = 6;
	orderType = cache_bitmap->compressed ? ORDER_TYPE_CACHE_BITMAP_COMPRESSED
	                                     : ORDER_TYPE_BITMAP_UNCOMPRESSED;
	inf =
	    update_approximate_cache_bitmap_order(cache_bitmap, cache_bitmap->compressed, &extraFlags);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	bm = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);

	if (!update_write_cache_bitmap_order(s, cache_bitmap, cache_bitmap->compressed, &extraFlags))
		return FALSE;

	em = Stream_GetPosition(s);
	orderLength = (em - bm) - 13;
	Stream_SetPosition(s, bm);
	Stream_Write_UINT8(s, ORDER_STANDARD | ORDER_SECONDARY); /* controlFlags (1 byte) */
	Stream_Write_UINT16(s, orderLength);                     /* orderLength (2 bytes) */
	Stream_Write_UINT16(s, extraFlags);                      /* extraFlags (2 bytes) */
	Stream_Write_UINT8(s, orderType);                        /* orderType (1 byte) */
	Stream_SetPosition(s, em);
	update->numberOrders++;
	return TRUE;
}