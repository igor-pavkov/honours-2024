static UINT16 update_calculate_new_or_existing_notification_icons_order(
    const WINDOW_ORDER_INFO* orderInfo, const NOTIFY_ICON_STATE_ORDER* iconStateOrder)
{
	UINT16 orderSize = 15;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_VERSION) != 0)
		orderSize += 4;

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_TIP) != 0)
	{
		orderSize += 2 + iconStateOrder->toolTip.length;
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_INFO_TIP) != 0)
	{
		NOTIFY_ICON_INFOTIP infoTip = iconStateOrder->infoTip;
		orderSize += 12 + infoTip.text.length + infoTip.title.length;
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_STATE) != 0)
	{
		orderSize += 4;
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_ICON) != 0)
	{
		ICON_INFO iconInfo = iconStateOrder->icon;
		orderSize += 12;

		if (iconInfo.bpp <= 8)
			orderSize += 2 + iconInfo.cbColorTable;

		orderSize += iconInfo.cbBitsMask + iconInfo.cbBitsColor;
	}
	else if ((orderInfo->fieldFlags & WINDOW_ORDER_CACHED_ICON) != 0)
	{
		orderSize += 3;
	}

	return orderSize;
}