static UINT16 update_calculate_monitored_desktop(const WINDOW_ORDER_INFO* orderInfo,
                                                 const MONITORED_DESKTOP_ORDER* monitoredDesktop)
{
	UINT16 orderSize = 7;

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_DESKTOP_ACTIVE_WND)
	{
		orderSize += 4;
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_DESKTOP_ZORDER)
	{
		orderSize += 1 + (4 * monitoredDesktop->numWindowIds);
	}

	return orderSize;
}