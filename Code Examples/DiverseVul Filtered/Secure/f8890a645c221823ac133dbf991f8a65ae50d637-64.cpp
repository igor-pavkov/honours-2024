static BOOL update_send_cache_glyph(rdpContext* context, const CACHE_GLYPH_ORDER* cache_glyph)
{
	wStream* s;
	UINT16 flags;
	size_t bm, em, inf;
	int headerLength;
	INT16 orderLength;
	rdpUpdate* update = context->update;
	flags = 0;
	headerLength = 6;
	inf = update_approximate_cache_glyph_order(cache_glyph, &flags);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	bm = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);

	if (!update_write_cache_glyph_order(s, cache_glyph, &flags))
		return FALSE;

	em = Stream_GetPosition(s);
	orderLength = (em - bm) - 13;
	Stream_SetPosition(s, bm);
	Stream_Write_UINT8(s, ORDER_STANDARD | ORDER_SECONDARY); /* controlFlags (1 byte) */
	Stream_Write_UINT16(s, orderLength);                     /* orderLength (2 bytes) */
	Stream_Write_UINT16(s, flags);                           /* extraFlags (2 bytes) */
	Stream_Write_UINT8(s, ORDER_TYPE_CACHE_GLYPH);           /* orderType (1 byte) */
	Stream_SetPosition(s, em);
	update->numberOrders++;
	return TRUE;
}