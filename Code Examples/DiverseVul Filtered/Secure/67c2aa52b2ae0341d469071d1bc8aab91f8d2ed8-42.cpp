static BOOL update_read_draw_nine_grid_order(wStream* s, const ORDER_INFO* orderInfo,
                                             DRAW_NINE_GRID_ORDER* draw_nine_grid)
{
	ORDER_FIELD_COORD(1, draw_nine_grid->srcLeft);
	ORDER_FIELD_COORD(2, draw_nine_grid->srcTop);
	ORDER_FIELD_COORD(3, draw_nine_grid->srcRight);
	ORDER_FIELD_COORD(4, draw_nine_grid->srcBottom);
	ORDER_FIELD_UINT16(5, draw_nine_grid->bitmapId);
	return TRUE;
}