static void dump_monitored_desktop(wLog* log, const char* msg, const WINDOW_ORDER_INFO* orderInfo,
                                   const MONITORED_DESKTOP_ORDER* monitored)
{
	char buffer[1000] = { 0 };
	const size_t bufferSize = sizeof(buffer) - 1;

	DUMP_APPEND(buffer, bufferSize, "%s", msg);

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_DESKTOP_ACTIVE_WND)
		DUMP_APPEND(buffer, bufferSize, " activeWindowId=0x%" PRIx32 "", monitored->activeWindowId);

	if (orderInfo->fieldFlags & WINDOW_ORDER_FIELD_DESKTOP_ZORDER)
	{
		UINT32 i;

		DUMP_APPEND(buffer, bufferSize, " windows=(");
		for (i = 0; i < monitored->numWindowIds; i++)
		{
			DUMP_APPEND(buffer, bufferSize, "0x%" PRIx32 ",", monitored->windowIds[i]);
		}
		DUMP_APPEND(buffer, bufferSize, ")");
	}
	WLog_Print(log, WLOG_DEBUG, buffer);
}