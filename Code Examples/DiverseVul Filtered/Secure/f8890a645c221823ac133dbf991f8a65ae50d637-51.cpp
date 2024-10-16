static UINT16 update_calculate_window_icon_order(const WINDOW_ORDER_INFO* orderInfo,
                                                 const WINDOW_ICON_ORDER* iconOrder)
{
	UINT16 orderSize = 23;
	ICON_INFO* iconInfo = iconOrder->iconInfo;

	orderSize += iconInfo->cbBitsColor + iconInfo->cbBitsMask;

	if (iconInfo->bpp <= 8)
		orderSize += 2 + iconInfo->cbColorTable;

	return orderSize;
}