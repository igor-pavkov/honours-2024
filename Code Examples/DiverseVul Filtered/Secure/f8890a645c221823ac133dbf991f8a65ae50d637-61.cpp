static BOOL update_send_cache_brush(rdpContext* context, const CACHE_BRUSH_ORDER* cache_brush)
{
	wStream* s;
	UINT16 flags;
	size_t bm, em, inf;
	int headerLength;
	INT16 orderLength;
	rdpUpdate* update = context->update;
	flags = 0;
	headerLength = 6;
	inf = update_approximate_cache_brush_order(cache_brush, &flags);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	bm = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);

	if (!update_write_cache_brush_order(s, cache_brush, &flags))
		return FALSE;

	em = Stream_GetPosition(s);
	orderLength = (em - bm) - 13;
	Stream_SetPosition(s, bm);
	Stream_Write_UINT8(s, ORDER_STANDARD | ORDER_SECONDARY); /* controlFlags (1 byte) */
	Stream_Write_UINT16(s, orderLength);                     /* orderLength (2 bytes) */
	Stream_Write_UINT16(s, flags);                           /* extraFlags (2 bytes) */
	Stream_Write_UINT8(s, ORDER_TYPE_CACHE_BRUSH);           /* orderType (1 byte) */
	Stream_SetPosition(s, em);
	update->numberOrders++;
	return TRUE;
}