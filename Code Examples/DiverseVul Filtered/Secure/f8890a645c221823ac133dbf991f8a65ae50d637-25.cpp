static BOOL update_send_patblt(rdpContext* context, PATBLT_ORDER* patblt)
{
	wStream* s;
	size_t offset;
	int headerLength;
	ORDER_INFO orderInfo;
	rdpUpdate* update = context->update;
	headerLength = update_prepare_order_info(context, &orderInfo, ORDER_TYPE_PATBLT);
	update_check_flush(context, headerLength + update_approximate_patblt_order(&orderInfo, patblt));
	s = update->us;

	if (!s)
		return FALSE;

	offset = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);
	update_write_patblt_order(s, &orderInfo, patblt);
	update_write_order_info(context, s, &orderInfo, offset);
	update->numberOrders++;
	return TRUE;
}