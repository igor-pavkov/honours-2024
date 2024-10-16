static BOOL update_recv_window_info_order(rdpUpdate* update, wStream* s,
                                          WINDOW_ORDER_INFO* orderInfo)
{
	rdpContext* context = update->context;
	rdpWindowUpdate* window = update->window;
	BOOL result = TRUE;

	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	Stream_Read_UINT32(s, orderInfo->windowId); /* windowId (4 bytes) */

	if (orderInfo->fieldFlags & WINDOW_ORDER_ICON)
	{
		WINDOW_ICON_ORDER window_icon = { 0 };
		result = update_read_window_icon_order(s, orderInfo, &window_icon);

		if (result)
		{
			WLog_Print(update->log, WLOG_DEBUG, "WindowIcon windowId=0x%" PRIx32 "",
			           orderInfo->windowId);
			IFCALLRET(window->WindowIcon, result, context, orderInfo, &window_icon);
		}

		update_free_window_icon_info(window_icon.iconInfo);
		free(window_icon.iconInfo);
	}
	else if (orderInfo->fieldFlags & WINDOW_ORDER_CACHED_ICON)
	{
		WINDOW_CACHED_ICON_ORDER window_cached_icon = { 0 };
		result = update_read_window_cached_icon_order(s, orderInfo, &window_cached_icon);

		if (result)
		{
			WLog_Print(update->log, WLOG_DEBUG, "WindowCachedIcon windowId=0x%" PRIx32 "",
			           orderInfo->windowId);
			IFCALLRET(window->WindowCachedIcon, result, context, orderInfo, &window_cached_icon);
		}
	}
	else if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_DELETED)
	{
		update_read_window_delete_order(s, orderInfo);
		WLog_Print(update->log, WLOG_DEBUG, "WindowDelete windowId=0x%" PRIx32 "",
		           orderInfo->windowId);
		IFCALLRET(window->WindowDelete, result, context, orderInfo);
	}
	else
	{
		WINDOW_STATE_ORDER windowState = { 0 };
		result = update_read_window_state_order(s, orderInfo, &windowState);

		if (result)
		{
			if (orderInfo->fieldFlags & WINDOW_ORDER_STATE_NEW)
			{
				dump_window_state_order(update->log, "WindowCreate", orderInfo, &windowState);
				IFCALLRET(window->WindowCreate, result, context, orderInfo, &windowState);
			}
			else
			{
				dump_window_state_order(update->log, "WindowUpdate", orderInfo, &windowState);
				IFCALLRET(window->WindowUpdate, result, context, orderInfo, &windowState);
			}

			update_free_window_state(&windowState);
		}
	}

	return result;
}