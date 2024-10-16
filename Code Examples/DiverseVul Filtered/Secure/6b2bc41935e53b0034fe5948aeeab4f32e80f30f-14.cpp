static BOOL update_read_window_icon_order(wStream* s, WINDOW_ORDER_INFO* orderInfo,
                                          WINDOW_ICON_ORDER* window_icon)
{
	WINPR_UNUSED(orderInfo);
	window_icon->iconInfo = (ICON_INFO*)calloc(1, sizeof(ICON_INFO));

	if (!window_icon->iconInfo)
		return FALSE;

	return update_read_icon_info(s, window_icon->iconInfo); /* iconInfo (ICON_INFO) */
}