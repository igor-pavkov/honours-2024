static BOOL check_alt_order_supported(wLog* log, rdpSettings* settings, BYTE orderType,
                                      const char* orderName)
{
	BOOL condition = FALSE;

	switch (orderType)
	{
		case ORDER_TYPE_CREATE_OFFSCREEN_BITMAP:
		case ORDER_TYPE_SWITCH_SURFACE:
			condition = settings->OffscreenSupportLevel != 0;
			break;

		case ORDER_TYPE_CREATE_NINE_GRID_BITMAP:
			condition = settings->DrawNineGridEnabled;
			break;

		case ORDER_TYPE_FRAME_MARKER:
			condition = settings->FrameMarkerCommandEnabled;
			break;

		case ORDER_TYPE_GDIPLUS_FIRST:
		case ORDER_TYPE_GDIPLUS_NEXT:
		case ORDER_TYPE_GDIPLUS_END:
		case ORDER_TYPE_GDIPLUS_CACHE_FIRST:
		case ORDER_TYPE_GDIPLUS_CACHE_NEXT:
		case ORDER_TYPE_GDIPLUS_CACHE_END:
			condition = settings->DrawGdiPlusCacheEnabled;
			break;

		case ORDER_TYPE_WINDOW:
			condition = settings->RemoteWndSupportLevel != WINDOW_LEVEL_NOT_SUPPORTED;
			break;

		case ORDER_TYPE_STREAM_BITMAP_FIRST:
		case ORDER_TYPE_STREAM_BITMAP_NEXT:
		case ORDER_TYPE_COMPDESK_FIRST:
			condition = TRUE;
			break;

		default:
			WLog_Print(log, WLOG_WARN, "%s - Alternate Secondary Drawing Order UNKNOWN", orderName);
			condition = FALSE;
			break;
	}

	return check_order_activated(log, settings, orderName, condition);
}