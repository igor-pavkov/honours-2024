static BOOL update_send_notify_icon_delete(rdpContext* context, const WINDOW_ORDER_INFO* orderInfo)
{
	wStream* s;
	rdpUpdate* update = context->update;
	BYTE controlFlags = ORDER_SECONDARY | (ORDER_TYPE_WINDOW << 2);
	UINT16 orderSize = 15;
	update_check_flush(context, orderSize);

	s = update->us;

	if (!s)
		return FALSE;

	/* Write Hdr */
	Stream_Write_UINT8(s, controlFlags);             /* Header (1 byte) */
	Stream_Write_UINT16(s, orderSize);               /* OrderSize (2 bytes) */
	Stream_Write_UINT32(s, orderInfo->fieldFlags);   /* FieldsPresentFlags (4 bytes) */
	Stream_Write_UINT32(s, orderInfo->windowId);     /* WindowID (4 bytes) */
	Stream_Write_UINT32(s, orderInfo->notifyIconId); /* NotifyIconId (4 bytes) */
	update->numberOrders++;
	return TRUE;
}