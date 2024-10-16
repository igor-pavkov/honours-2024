static BOOL update_send_create_offscreen_bitmap_order(
    rdpContext* context, const CREATE_OFFSCREEN_BITMAP_ORDER* create_offscreen_bitmap)
{
	wStream* s;
	size_t bm, em, inf;
	BYTE orderType;
	BYTE controlFlags;
	int headerLength;
	rdpUpdate* update = context->update;
	headerLength = 1;
	orderType = ORDER_TYPE_CREATE_OFFSCREEN_BITMAP;
	controlFlags = ORDER_SECONDARY | (orderType << 2);
	inf = update_approximate_create_offscreen_bitmap_order(create_offscreen_bitmap);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	bm = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);

	if (!update_write_create_offscreen_bitmap_order(s, create_offscreen_bitmap))
		return FALSE;

	em = Stream_GetPosition(s);
	Stream_SetPosition(s, bm);
	Stream_Write_UINT8(s, controlFlags); /* controlFlags (1 byte) */
	Stream_SetPosition(s, em);
	update->numberOrders++;
	return TRUE;
}