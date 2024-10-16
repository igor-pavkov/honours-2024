static BOOL _update_end_paint(rdpContext* context)
{
	wStream* s;
	int headerLength;
	rdpUpdate* update = context->update;

	if (!update->us)
		return FALSE;

	s = update->us;
	headerLength = Stream_Length(s);
	Stream_SealLength(s);
	Stream_SetPosition(s, headerLength);
	Stream_Write_UINT16(s, update->numberOrders); /* numberOrders (2 bytes) */
	Stream_SetPosition(s, Stream_Length(s));

	if (update->numberOrders > 0)
	{
		WLog_DBG(TAG, "sending %" PRIu16 " orders", update->numberOrders);
		fastpath_send_update_pdu(context->rdp->fastpath, FASTPATH_UPDATETYPE_ORDERS, s, FALSE);
	}

	update->combineUpdates = FALSE;
	update->numberOrders = 0;
	update->us = NULL;
	Stream_Free(s, TRUE);
	return TRUE;
}