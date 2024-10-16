BOOL update_recv_altsec_window_order(rdpUpdate* update, wStream* s)
{
	BOOL rc = TRUE;
	size_t remaining;
	UINT16 orderSize;
	WINDOW_ORDER_INFO orderInfo = { 0 };
	remaining = Stream_GetRemainingLength(s);

	if (remaining < 6)
	{
		WLog_Print(update->log, WLOG_ERROR, "Stream short");
		return FALSE;
	}

	Stream_Read_UINT16(s, orderSize);            /* orderSize (2 bytes) */
	Stream_Read_UINT32(s, orderInfo.fieldFlags); /* FieldsPresentFlags (4 bytes) */

	if (remaining + 1 < orderSize)
	{
		WLog_Print(update->log, WLOG_ERROR, "Stream short orderSize");
		return FALSE;
	}

	if (!window_order_supported(update->context->settings, orderInfo.fieldFlags))
	{
		WLog_INFO(TAG, "Window order %08" PRIx32 " not supported!", orderInfo.fieldFlags);
		return FALSE;
	}

	if (orderInfo.fieldFlags & WINDOW_ORDER_TYPE_WINDOW)
		rc = update_recv_window_info_order(update, s, &orderInfo);
	else if (orderInfo.fieldFlags & WINDOW_ORDER_TYPE_NOTIFY)
		rc = update_recv_notification_icon_info_order(update, s, &orderInfo);
	else if (orderInfo.fieldFlags & WINDOW_ORDER_TYPE_DESKTOP)
		rc = update_recv_desktop_info_order(update, s, &orderInfo);

	if (!rc)
		WLog_Print(update->log, WLOG_ERROR, "windoworder flags %08" PRIx32 " failed",
		           orderInfo.fieldFlags);

	return rc;
}