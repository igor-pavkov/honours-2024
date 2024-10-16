static BOOL update_send_glyph_index(rdpContext* context, GLYPH_INDEX_ORDER* glyph_index)
{
	wStream* s;
	size_t offset;
	int headerLength;
	int inf;
	ORDER_INFO orderInfo;
	rdpUpdate* update = context->update;
	headerLength = update_prepare_order_info(context, &orderInfo, ORDER_TYPE_GLYPH_INDEX);
	inf = update_approximate_glyph_index_order(&orderInfo, glyph_index);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	offset = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);
	update_write_glyph_index_order(s, &orderInfo, glyph_index);
	update_write_order_info(context, s, &orderInfo, offset);
	update->numberOrders++;
	return TRUE;
}