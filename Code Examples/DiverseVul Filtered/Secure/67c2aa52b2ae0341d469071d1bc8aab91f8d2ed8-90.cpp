static BOOL update_recv_altsec_order(rdpUpdate* update, wStream* s, BYTE flags)
{
	BYTE orderType = flags >>= 2; /* orderType is in higher 6 bits of flags field */
	BOOL rc = FALSE;
	rdpContext* context = update->context;
	rdpSettings* settings = context->settings;
	rdpAltSecUpdate* altsec = update->altsec;
	const char* orderName = altsec_order_string(orderType);
	WLog_Print(update->log, WLOG_DEBUG, "Alternate Secondary Drawing Order %s", orderName);

	if (!check_alt_order_supported(update->log, settings, orderType, orderName))
		return FALSE;

	if (!read_altsec_order(s, orderType, altsec))
		return FALSE;

	switch (orderType)
	{
		case ORDER_TYPE_CREATE_OFFSCREEN_BITMAP:
			IFCALLRET(altsec->CreateOffscreenBitmap, rc, context,
			          &(altsec->create_offscreen_bitmap));
			break;

		case ORDER_TYPE_SWITCH_SURFACE:
			IFCALLRET(altsec->SwitchSurface, rc, context, &(altsec->switch_surface));
			break;

		case ORDER_TYPE_CREATE_NINE_GRID_BITMAP:
			IFCALLRET(altsec->CreateNineGridBitmap, rc, context,
			          &(altsec->create_nine_grid_bitmap));
			break;

		case ORDER_TYPE_FRAME_MARKER:
			IFCALLRET(altsec->FrameMarker, rc, context, &(altsec->frame_marker));
			break;

		case ORDER_TYPE_STREAM_BITMAP_FIRST:
			IFCALLRET(altsec->StreamBitmapFirst, rc, context, &(altsec->stream_bitmap_first));
			break;

		case ORDER_TYPE_STREAM_BITMAP_NEXT:
			IFCALLRET(altsec->StreamBitmapNext, rc, context, &(altsec->stream_bitmap_next));
			break;

		case ORDER_TYPE_GDIPLUS_FIRST:
			IFCALLRET(altsec->DrawGdiPlusFirst, rc, context, &(altsec->draw_gdiplus_first));
			break;

		case ORDER_TYPE_GDIPLUS_NEXT:
			IFCALLRET(altsec->DrawGdiPlusNext, rc, context, &(altsec->draw_gdiplus_next));
			break;

		case ORDER_TYPE_GDIPLUS_END:
			IFCALLRET(altsec->DrawGdiPlusEnd, rc, context, &(altsec->draw_gdiplus_end));
			break;

		case ORDER_TYPE_GDIPLUS_CACHE_FIRST:
			IFCALLRET(altsec->DrawGdiPlusCacheFirst, rc, context,
			          &(altsec->draw_gdiplus_cache_first));
			break;

		case ORDER_TYPE_GDIPLUS_CACHE_NEXT:
			IFCALLRET(altsec->DrawGdiPlusCacheNext, rc, context,
			          &(altsec->draw_gdiplus_cache_next));
			break;

		case ORDER_TYPE_GDIPLUS_CACHE_END:
			IFCALLRET(altsec->DrawGdiPlusCacheEnd, rc, context, &(altsec->draw_gdiplus_cache_end));
			break;

		case ORDER_TYPE_WINDOW:
			rc = update_recv_altsec_window_order(update, s);
			break;

		case ORDER_TYPE_COMPDESK_FIRST:
			rc = TRUE;
			break;

		default:
			break;
	}

	if (!rc)
	{
		WLog_Print(update->log, WLOG_WARN, "Alternate Secondary Drawing Order %s failed",
		           orderName);
	}

	return rc;
}