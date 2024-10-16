static BOOL update_send_switch_surface_order(rdpContext* context,
                                             const SWITCH_SURFACE_ORDER* switch_surface)
{
	wStream* s;
	size_t bm, em, inf;
	BYTE orderType;
	BYTE controlFlags;
	int headerLength;
	rdpUpdate* update;

	if (!context || !switch_surface || !context->update)
		return FALSE;

	update = context->update;
	headerLength = 1;
	orderType = ORDER_TYPE_SWITCH_SURFACE;
	controlFlags = ORDER_SECONDARY | (orderType << 2);
	inf = update_approximate_switch_surface_order(switch_surface);
	update_check_flush(context, headerLength + inf);
	s = update->us;

	if (!s)
		return FALSE;

	bm = Stream_GetPosition(s);

	if (!Stream_EnsureRemainingCapacity(s, headerLength))
		return FALSE;

	Stream_Seek(s, headerLength);

	if (!update_write_switch_surface_order(s, switch_surface))
		return FALSE;

	em = Stream_GetPosition(s);
	Stream_SetPosition(s, bm);
	Stream_Write_UINT8(s, controlFlags); /* controlFlags (1 byte) */
	Stream_SetPosition(s, em);
	update->numberOrders++;
	return TRUE;
}