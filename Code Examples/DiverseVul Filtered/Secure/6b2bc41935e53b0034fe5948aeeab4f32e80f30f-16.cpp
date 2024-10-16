static BOOL update_read_window_cached_icon_order(wStream* s, WINDOW_ORDER_INFO* orderInfo,
                                                 WINDOW_CACHED_ICON_ORDER* window_cached_icon)
{
	WINPR_UNUSED(orderInfo);
	return update_read_cached_icon_info(
	    s, &window_cached_icon->cachedIcon); /* cachedIcon (CACHED_ICON_INFO) */
}