static BOOL check_primary_order_supported(wLog* log, rdpSettings* settings, UINT32 orderType,
                                          const char* orderName)
{
	BOOL condition = FALSE;

	switch (orderType)
	{
		case ORDER_TYPE_DSTBLT:
			condition = settings->OrderSupport[NEG_DSTBLT_INDEX];
			break;

		case ORDER_TYPE_SCRBLT:
			condition = settings->OrderSupport[NEG_SCRBLT_INDEX];
			break;

		case ORDER_TYPE_DRAW_NINE_GRID:
			condition = settings->OrderSupport[NEG_DRAWNINEGRID_INDEX];
			break;

		case ORDER_TYPE_MULTI_DRAW_NINE_GRID:
			condition = settings->OrderSupport[NEG_MULTI_DRAWNINEGRID_INDEX];
			break;

		case ORDER_TYPE_LINE_TO:
			condition = settings->OrderSupport[NEG_LINETO_INDEX];
			break;

		/* [MS-RDPEGDI] 2.2.2.2.1.1.2.5 OpaqueRect (OPAQUERECT_ORDER)
		 * suggests that PatBlt and OpaqueRect imply each other. */
		case ORDER_TYPE_PATBLT:
		case ORDER_TYPE_OPAQUE_RECT:
			condition = settings->OrderSupport[NEG_OPAQUE_RECT_INDEX] ||
			            settings->OrderSupport[NEG_PATBLT_INDEX];
			break;

		case ORDER_TYPE_SAVE_BITMAP:
			condition = settings->OrderSupport[NEG_SAVEBITMAP_INDEX];
			break;

		case ORDER_TYPE_MEMBLT:
			condition = settings->OrderSupport[NEG_MEMBLT_INDEX];
			break;

		case ORDER_TYPE_MEM3BLT:
			condition = settings->OrderSupport[NEG_MEM3BLT_INDEX];
			break;

		case ORDER_TYPE_MULTI_DSTBLT:
			condition = settings->OrderSupport[NEG_MULTIDSTBLT_INDEX];
			break;

		case ORDER_TYPE_MULTI_PATBLT:
			condition = settings->OrderSupport[NEG_MULTIPATBLT_INDEX];
			break;

		case ORDER_TYPE_MULTI_SCRBLT:
			condition = settings->OrderSupport[NEG_MULTIDSTBLT_INDEX];
			break;

		case ORDER_TYPE_MULTI_OPAQUE_RECT:
			condition = settings->OrderSupport[NEG_MULTIOPAQUERECT_INDEX];
			break;

		case ORDER_TYPE_FAST_INDEX:
			condition = settings->OrderSupport[NEG_FAST_INDEX_INDEX];
			break;

		case ORDER_TYPE_POLYGON_SC:
			condition = settings->OrderSupport[NEG_POLYGON_SC_INDEX];
			break;

		case ORDER_TYPE_POLYGON_CB:
			condition = settings->OrderSupport[NEG_POLYGON_CB_INDEX];
			break;

		case ORDER_TYPE_POLYLINE:
			condition = settings->OrderSupport[NEG_POLYLINE_INDEX];
			break;

		case ORDER_TYPE_FAST_GLYPH:
			condition = settings->OrderSupport[NEG_FAST_GLYPH_INDEX];
			break;

		case ORDER_TYPE_ELLIPSE_SC:
			condition = settings->OrderSupport[NEG_ELLIPSE_SC_INDEX];
			break;

		case ORDER_TYPE_ELLIPSE_CB:
			condition = settings->OrderSupport[NEG_ELLIPSE_CB_INDEX];
			break;

		case ORDER_TYPE_GLYPH_INDEX:
			condition = settings->OrderSupport[NEG_GLYPH_INDEX_INDEX];
			break;

		default:
			WLog_Print(log, WLOG_WARN, "%s Primary Drawing Order not supported", orderName);
			break;
	}

	return check_order_activated(log, settings, orderName, condition);
}