static BOOL update_send_dstblt(rdpContext* context, const DSTBLT_ORDER* dstblt)
{
	wStream* s;
	UINT32 offset;
	UINT32 headerLength;
	ORDER_INFO orderInfo;
	int inf;
	rdpUpdate* update = context->update;
	headerLength = update_prepare_order_info(context, &orderInfo, ORDER_TYPE_DSTBLT);
	inf = update_approximate_dstblt_order(&orderInfo, dstblt);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	offset = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);

	if (!update_write_dstblt_order(s, &orderInfo, dstblt))
		return FALSE;

	update_write_order_info(context, s, &orderInfo, offset);
	update->numberOrders++;
	return TRUE;
}