static BOOL update_send_cache_bitmap_v3(rdpContext* context, CACHE_BITMAP_V3_ORDER* cache_bitmap_v3)
{
	wStream* s;
	size_t bm, em;
	BYTE orderType;
	int headerLength;
	UINT16 extraFlags;
	INT16 orderLength;
	rdpUpdate* update = context->update;
	extraFlags = 0;
	headerLength = 6;
	orderType = ORDER_TYPE_BITMAP_COMPRESSED_V3;
	update_check_flush(context, headerLength + update_approximate_cache_bitmap_v3_order(
	                                               cache_bitmap_v3, &extraFlags));
	s = update->us;

	if (!s)
		return FALSE;

	bm = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);

	if (!update_write_cache_bitmap_v3_order(s, cache_bitmap_v3, &extraFlags))
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