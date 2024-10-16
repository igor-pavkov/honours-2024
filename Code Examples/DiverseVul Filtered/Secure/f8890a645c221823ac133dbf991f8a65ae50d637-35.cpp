static BOOL update_send_window_cached_icon(rdpContext* context, const WINDOW_ORDER_INFO* orderInfo,
                                           const WINDOW_CACHED_ICON_ORDER* cachedIconOrder)
{
	wStream* s;
	rdpUpdate* update = context->update;
	BYTE controlFlags = ORDER_SECONDARY | (ORDER_TYPE_WINDOW << 2);
	UINT16 orderSize = 14;
	CACHED_ICON_INFO cachedIcon = cachedIconOrder->cachedIcon;

	update_check_flush(context, orderSize);

	s = update->us;
	if (!s)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(s, orderSize))
		return FALSE;

	/* Write Hdr */
	Stream_Write_UINT8(s, controlFlags);           /* Header (1 byte) */
	Stream_Write_UINT16(s, orderSize);             /* OrderSize (2 bytes) */
	Stream_Write_UINT32(s, orderInfo->fieldFlags); /* FieldsPresentFlags (4 bytes) */
	Stream_Write_UINT32(s, orderInfo->windowId);   /* WindowID (4 bytes) */
	/* Write body */
	Stream_Write_UINT16(s, cachedIcon.cacheEntry); /* CacheEntry (2 bytes) */
	Stream_Write_UINT8(s, cachedIcon.cacheId);     /* CacheId (1 byte) */
	update->numberOrders++;
	return TRUE;
}