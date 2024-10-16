static BOOL update_recv_desktop_info_order(rdpUpdate* update, wStream* s,
                                           WINDOW_ORDER_INFO* orderInfo)
{
	rdpContext* context = update->context;
	rdpWindowUpdate* window = update->window;
	BOOL result = TRUE;

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_DESKTOP_NONE)
	{
		update_read_desktop_non_monitored_order(s, orderInfo);
		WLog_Print(update->log, WLOG_DEBUG, "NonMonitoredDesktop, windowId=0x%" PRIx32 "",
		           orderInfo->windowId);
		IFCALLRET(window->NonMonitoredDesktop, result, context, orderInfo);
	}
	else
	{
		MONITORED_DESKTOP_ORDER monitored_desktop = { 0 };
		result = update_read_desktop_actively_monitored_order(s, orderInfo, &monitored_desktop);

		if (result)
		{
			dump_monitored_desktop(update->log, "ActivelyMonitoredDesktop", orderInfo,
			                       &monitored_desktop);
			IFCALLRET(window->MonitoredDesktop, result, context, orderInfo, &monitored_desktop);
		}

		free(monitored_desktop.windowIds);
	}

	return result;
}