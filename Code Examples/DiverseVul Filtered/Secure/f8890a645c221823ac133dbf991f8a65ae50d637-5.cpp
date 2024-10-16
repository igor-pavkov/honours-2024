static BOOL update_send_non_monitored_desktop(rdpContext* context,
                                              const WINDOW_ORDER_INFO* orderInfo)
{
	wStream* s;
	rdpUpdate* update = context->update;
	BYTE controlFlags = ORDER_SECONDARY | (ORDER_TYPE_WINDOW << 2);
	UINT16 orderSize = 7;
	update_check_flush(context, orderSize);

	s = update->us;

	if (!s)
		return FALSE;

	Stream_Write_UINT8(s, controlFlags);           /* Header (1 byte) */
	Stream_Write_UINT16(s, orderSize);             /* OrderSize (2 bytes) */
	Stream_Write_UINT32(s, orderInfo->fieldFlags); /* FieldsPresentFlags (4 bytes) */
	update->numberOrders++;
	return TRUE;
}