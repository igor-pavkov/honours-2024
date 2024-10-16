static BOOL update_send_cache_color_table(rdpContext* context,
                                          const CACHE_COLOR_TABLE_ORDER* cache_color_table)
{
	wStream* s;
	UINT16 flags;
	size_t bm, em, inf;
	int headerLength;
	INT16 orderLength;
	rdpUpdate* update = context->update;
	flags = 0;
	headerLength = 6;
	inf = update_approximate_cache_color_table_order(cache_color_table, &flags);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	bm = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);

	if (!update_write_cache_color_table_order(s, cache_color_table, &flags))
		return FALSE;

	em = Stream_GetPosition(s);
	orderLength = (em - bm) - 13;
	Stream_SetPosition(s, bm);
	Stream_Write_UINT8(s, ORDER_STANDARD | ORDER_SECONDARY); /* controlFlags (1 byte) */
	Stream_Write_UINT16(s, orderLength);                     /* orderLength (2 bytes) */
	Stream_Write_UINT16(s, flags);                           /* extraFlags (2 bytes) */
	Stream_Write_UINT8(s, ORDER_TYPE_CACHE_COLOR_TABLE);     /* orderType (1 byte) */
	Stream_SetPosition(s, em);
	update->numberOrders++;
	return TRUE;
}