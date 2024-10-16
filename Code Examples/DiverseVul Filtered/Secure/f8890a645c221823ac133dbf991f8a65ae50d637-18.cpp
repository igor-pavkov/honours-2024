static BOOL update_send_scrblt(rdpContext* context, const SCRBLT_ORDER* scrblt)
{
	wStream* s;
	UINT32 offset;
	UINT32 headerLength;
	ORDER_INFO orderInfo;
	int inf;
	rdpUpdate* update = context->update;
	headerLength = update_prepare_order_info(context, &orderInfo, ORDER_TYPE_SCRBLT);
	inf = update_approximate_scrblt_order(&orderInfo, scrblt);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return TRUE;

	offset = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);
	update_write_scrblt_order(s, &orderInfo, scrblt);
	update_write_order_info(context, s, &orderInfo, offset);
	update->numberOrders++;
	return TRUE;
}