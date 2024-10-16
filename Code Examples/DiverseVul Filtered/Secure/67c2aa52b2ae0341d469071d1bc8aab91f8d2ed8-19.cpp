static BOOL check_secondary_order_supported(wLog* log, rdpSettings* settings, BYTE orderType,
                                            const char* orderName)
{
	BOOL condition = FALSE;

	switch (orderType)
	{
		case ORDER_TYPE_BITMAP_UNCOMPRESSED:
		case ORDER_TYPE_CACHE_BITMAP_COMPRESSED:
			condition = settings->BitmapCacheEnabled;
			break;

		case ORDER_TYPE_BITMAP_UNCOMPRESSED_V2:
		case ORDER_TYPE_BITMAP_COMPRESSED_V2:
			condition = settings->BitmapCacheEnabled;
			break;

		case ORDER_TYPE_BITMAP_COMPRESSED_V3:
			condition = settings->BitmapCacheV3Enabled;
			break;

		case ORDER_TYPE_CACHE_COLOR_TABLE:
			condition = (settings->OrderSupport[NEG_MEMBLT_INDEX] ||
			             settings->OrderSupport[NEG_MEM3BLT_INDEX]);
			break;

		case ORDER_TYPE_CACHE_GLYPH:
		{
			switch (settings->GlyphSupportLevel)
			{
				case GLYPH_SUPPORT_PARTIAL:
				case GLYPH_SUPPORT_FULL:
				case GLYPH_SUPPORT_ENCODE:
					condition = TRUE;
					break;

				case GLYPH_SUPPORT_NONE:
				default:
					condition = FALSE;
					break;
			}
		}
		break;

		case ORDER_TYPE_CACHE_BRUSH:
			condition = TRUE;
			break;

		default:
			WLog_Print(log, WLOG_WARN, "SECONDARY ORDER %s not supported", orderName);
			break;
	}

	return check_order_activated(log, settings, orderName, condition);
}