static BOOL read_altsec_order(wStream* s, BYTE orderType, rdpAltSecUpdate* altsec)
{
	BOOL rc = FALSE;

	switch (orderType)
	{
		case ORDER_TYPE_CREATE_OFFSCREEN_BITMAP:
			rc = update_read_create_offscreen_bitmap_order(s, &(altsec->create_offscreen_bitmap));
			break;

		case ORDER_TYPE_SWITCH_SURFACE:
			rc = update_read_switch_surface_order(s, &(altsec->switch_surface));
			break;

		case ORDER_TYPE_CREATE_NINE_GRID_BITMAP:
			rc = update_read_create_nine_grid_bitmap_order(s, &(altsec->create_nine_grid_bitmap));
			break;

		case ORDER_TYPE_FRAME_MARKER:
			rc = update_read_frame_marker_order(s, &(altsec->frame_marker));
			break;

		case ORDER_TYPE_STREAM_BITMAP_FIRST:
			rc = update_read_stream_bitmap_first_order(s, &(altsec->stream_bitmap_first));
			break;

		case ORDER_TYPE_STREAM_BITMAP_NEXT:
			rc = update_read_stream_bitmap_next_order(s, &(altsec->stream_bitmap_next));
			break;

		case ORDER_TYPE_GDIPLUS_FIRST:
			rc = update_read_draw_gdiplus_first_order(s, &(altsec->draw_gdiplus_first));
			break;

		case ORDER_TYPE_GDIPLUS_NEXT:
			rc = update_read_draw_gdiplus_next_order(s, &(altsec->draw_gdiplus_next));
			break;

		case ORDER_TYPE_GDIPLUS_END:
			rc = update_read_draw_gdiplus_end_order(s, &(altsec->draw_gdiplus_end));
			break;

		case ORDER_TYPE_GDIPLUS_CACHE_FIRST:
			rc = update_read_draw_gdiplus_cache_first_order(s, &(altsec->draw_gdiplus_cache_first));
			break;

		case ORDER_TYPE_GDIPLUS_CACHE_NEXT:
			rc = update_read_draw_gdiplus_cache_next_order(s, &(altsec->draw_gdiplus_cache_next));
			break;

		case ORDER_TYPE_GDIPLUS_CACHE_END:
			rc = update_read_draw_gdiplus_cache_end_order(s, &(altsec->draw_gdiplus_cache_end));
			break;

		case ORDER_TYPE_WINDOW:
			/* This order is handled elsewhere. */
			rc = TRUE;
			break;

		case ORDER_TYPE_COMPDESK_FIRST:
			rc = TRUE;
			break;

		default:
			break;
	}

	return rc;
}