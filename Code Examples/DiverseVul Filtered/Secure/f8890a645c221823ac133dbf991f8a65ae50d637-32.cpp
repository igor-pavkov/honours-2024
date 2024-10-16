static BOOL update_send_memblt(rdpContext* context, MEMBLT_ORDER* memblt)
{
	wStream* s;
	size_t offset;
	int headerLength;
	ORDER_INFO orderInfo;
	rdpUpdate* update = context->update;
	headerLength = update_prepare_order_info(context, &orderInfo, ORDER_TYPE_MEMBLT);
	update_check_flush(context, headerLength + update_approximate_memblt_order(&orderInfo, memblt));
	s = update->us;

	if (!s)
		return FALSE;

	offset = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);
	update_write_memblt_order(s, &orderInfo, memblt);
	update_write_order_info(context, s, &orderInfo, offset);
	update->numberOrders++;
	return TRUE;
}