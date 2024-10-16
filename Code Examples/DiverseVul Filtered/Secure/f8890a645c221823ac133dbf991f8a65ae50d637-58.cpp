static BOOL update_send_window_icon(rdpContext* context, const WINDOW_ORDER_INFO* orderInfo,
                                    const WINDOW_ICON_ORDER* iconOrder)
{
	wStream* s;
	rdpUpdate* update = context->update;
	BYTE controlFlags = ORDER_SECONDARY | (ORDER_TYPE_WINDOW << 2);
	ICON_INFO* iconInfo = iconOrder->iconInfo;
	UINT16 orderSize = update_calculate_window_icon_order(orderInfo, iconOrder);

	update_check_flush(context, orderSize);

	s = update->us;

	if (!s || !iconInfo)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(s, orderSize))
		return FALSE;

	/* Write Hdr */
	Stream_Write_UINT8(s, controlFlags);           /* Header (1 byte) */
	Stream_Write_UINT16(s, orderSize);             /* OrderSize (2 bytes) */
	Stream_Write_UINT32(s, orderInfo->fieldFlags); /* FieldsPresentFlags (4 bytes) */
	Stream_Write_UINT32(s, orderInfo->windowId);   /* WindowID (4 bytes) */
	/* Write body */
	Stream_Write_UINT16(s, iconInfo->cacheEntry); /* CacheEntry (2 bytes) */
	Stream_Write_UINT8(s, iconInfo->cacheId);     /* CacheId (1 byte) */
	Stream_Write_UINT8(s, iconInfo->bpp);         /* Bpp (1 byte) */
	Stream_Write_UINT16(s, iconInfo->width);      /* Width (2 bytes) */
	Stream_Write_UINT16(s, iconInfo->height);     /* Height (2 bytes) */

	if (iconInfo->bpp <= 8)
	{
		Stream_Write_UINT16(s, iconInfo->cbColorTable); /* CbColorTable (2 bytes) */
	}

	Stream_Write_UINT16(s, iconInfo->cbBitsMask);              /* CbBitsMask (2 bytes) */
	Stream_Write_UINT16(s, iconInfo->cbBitsColor);             /* CbBitsColor (2 bytes) */
	Stream_Write(s, iconInfo->bitsMask, iconInfo->cbBitsMask); /* BitsMask (variable) */

	if (iconInfo->bpp <= 8)
	{
		Stream_Write(s, iconInfo->colorTable, iconInfo->cbColorTable); /* ColorTable (variable) */
	}

	Stream_Write(s, iconInfo->bitsColor, iconInfo->cbBitsColor); /* BitsColor (variable) */

	update->numberOrders++;
	return TRUE;
}