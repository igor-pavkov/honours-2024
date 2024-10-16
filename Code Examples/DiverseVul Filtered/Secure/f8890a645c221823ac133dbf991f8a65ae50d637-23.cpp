static BOOL update_send_opaque_rect(rdpContext* context, const OPAQUE_RECT_ORDER* opaque_rect)
{
	wStream* s;
	size_t offset;
	int headerLength;
	ORDER_INFO orderInfo;
	rdpUpdate* update = context->update;
	headerLength = update_prepare_order_info(context, &orderInfo, ORDER_TYPE_OPAQUE_RECT);
	update_check_flush(context, headerLength +
	                                update_approximate_opaque_rect_order(&orderInfo, opaque_rect));
	s = update->us;

	if (!s)
		return FALSE;

	offset = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);
	update_write_opaque_rect_order(s, &orderInfo, opaque_rect);
	update_write_order_info(context, s, &orderInfo, offset);
	update->numberOrders++;
	return TRUE;
}