static BOOL read_primary_order(wLog* log, const char* orderName, wStream* s,
                               const ORDER_INFO* orderInfo, rdpPrimaryUpdate* primary)
{
	BOOL rc = FALSE;

	if (!s || !orderInfo || !primary || !orderName)
		return FALSE;

	switch (orderInfo->orderType)
	{
		case ORDER_TYPE_DSTBLT:
			rc = update_read_dstblt_order(s, orderInfo, &(primary->dstblt));
			break;

		case ORDER_TYPE_PATBLT:
			rc = update_read_patblt_order(s, orderInfo, &(primary->patblt));
			break;

		case ORDER_TYPE_SCRBLT:
			rc = update_read_scrblt_order(s, orderInfo, &(primary->scrblt));
			break;

		case ORDER_TYPE_OPAQUE_RECT:
			rc = update_read_opaque_rect_order(s, orderInfo, &(primary->opaque_rect));
			break;

		case ORDER_TYPE_DRAW_NINE_GRID:
			rc = update_read_draw_nine_grid_order(s, orderInfo, &(primary->draw_nine_grid));
			break;

		case ORDER_TYPE_MULTI_DSTBLT:
			rc = update_read_multi_dstblt_order(s, orderInfo, &(primary->multi_dstblt));
			break;

		case ORDER_TYPE_MULTI_PATBLT:
			rc = update_read_multi_patblt_order(s, orderInfo, &(primary->multi_patblt));
			break;

		case ORDER_TYPE_MULTI_SCRBLT:
			rc = update_read_multi_scrblt_order(s, orderInfo, &(primary->multi_scrblt));
			break;

		case ORDER_TYPE_MULTI_OPAQUE_RECT:
			rc = update_read_multi_opaque_rect_order(s, orderInfo, &(primary->multi_opaque_rect));
			break;

		case ORDER_TYPE_MULTI_DRAW_NINE_GRID:
			rc = update_read_multi_draw_nine_grid_order(s, orderInfo,
			                                            &(primary->multi_draw_nine_grid));
			break;

		case ORDER_TYPE_LINE_TO:
			rc = update_read_line_to_order(s, orderInfo, &(primary->line_to));
			break;

		case ORDER_TYPE_POLYLINE:
			rc = update_read_polyline_order(s, orderInfo, &(primary->polyline));
			break;

		case ORDER_TYPE_MEMBLT:
			rc = update_read_memblt_order(s, orderInfo, &(primary->memblt));
			break;

		case ORDER_TYPE_MEM3BLT:
			rc = update_read_mem3blt_order(s, orderInfo, &(primary->mem3blt));
			break;

		case ORDER_TYPE_SAVE_BITMAP:
			rc = update_read_save_bitmap_order(s, orderInfo, &(primary->save_bitmap));
			break;

		case ORDER_TYPE_GLYPH_INDEX:
			rc = update_read_glyph_index_order(s, orderInfo, &(primary->glyph_index));
			break;

		case ORDER_TYPE_FAST_INDEX:
			rc = update_read_fast_index_order(s, orderInfo, &(primary->fast_index));
			break;

		case ORDER_TYPE_FAST_GLYPH:
			rc = update_read_fast_glyph_order(s, orderInfo, &(primary->fast_glyph));
			break;

		case ORDER_TYPE_POLYGON_SC:
			rc = update_read_polygon_sc_order(s, orderInfo, &(primary->polygon_sc));
			break;

		case ORDER_TYPE_POLYGON_CB:
			rc = update_read_polygon_cb_order(s, orderInfo, &(primary->polygon_cb));
			break;

		case ORDER_TYPE_ELLIPSE_SC:
			rc = update_read_ellipse_sc_order(s, orderInfo, &(primary->ellipse_sc));
			break;

		case ORDER_TYPE_ELLIPSE_CB:
			rc = update_read_ellipse_cb_order(s, orderInfo, &(primary->ellipse_cb));
			break;

		default:
			WLog_Print(log, WLOG_WARN, "Primary Drawing Order %s not supported, ignoring",
			           orderName);
			rc = TRUE;
			break;
	}

	if (!rc)
	{
		WLog_Print(log, WLOG_ERROR, "%s - update_read_dstblt_order() failed", orderName);
		return FALSE;
	}

	return TRUE;
}