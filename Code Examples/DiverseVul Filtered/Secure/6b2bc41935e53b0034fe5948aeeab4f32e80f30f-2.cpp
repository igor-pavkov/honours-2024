static BOOL update_read_notification_icon_state_order(wStream* s, WINDOW_ORDER_INFO* orderInfo,
                                                      NOTIFY_ICON_STATE_ORDER* notify_icon_state)
{
	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_VERSION)
	{
		if (Stream_GetRemainingLength(s) < 4)
			return FALSE;

		Stream_Read_UINT32(s, notify_icon_state->version); /* version (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_TIP)
	{
		if (!rail_read_unicode_string(s,
		                              &notify_icon_state->toolTip)) /* toolTip (UNICODE_STRING) */
			return FALSE;
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_INFO_TIP)
	{
		if (!update_read_notify_icon_infotip(
		        s, &notify_icon_state->infoTip)) /* infoTip (NOTIFY_ICON_INFOTIP) */
			return FALSE;
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_STATE)
	{
		if (Stream_GetRemainingLength(s) < 4)
			return FALSE;

		Stream_Read_UINT32(s, notify_icon_state->state); /* state (4 bytes) */
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_ICON)
	{
		if (!update_read_icon_info(s, &notify_icon_state->icon)) /* icon (ICON_INFO) */
			return FALSE;
	}

	if (orderInfo->fieldFlags & WINDOW_ORDER_CACHED_ICON)
	{
		if (!update_read_cached_icon_info(
		        s, &notify_icon_state->cachedIcon)) /* cachedIcon (CACHED_ICON_INFO) */
			return FALSE;
	}

	return TRUE;
}