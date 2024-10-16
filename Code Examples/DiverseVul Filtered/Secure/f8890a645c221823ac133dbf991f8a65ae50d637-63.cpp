static BOOL update_send_line_to(rdpContext* context, const LINE_TO_ORDER* line_to)
{
	wStream* s;
	int offset;
	int headerLength;
	ORDER_INFO orderInfo;
	int inf;
	rdpUpdate* update = context->update;
	headerLength = update_prepare_order_info(context, &orderInfo, ORDER_TYPE_LINE_TO);
	inf = update_approximate_line_to_order(&orderInfo, line_to);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	offset = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);
	update_write_line_to_order(s, &orderInfo, line_to);
	update_write_order_info(context, s, &orderInfo, offset);
	update->numberOrders++;
	return TRUE;
}