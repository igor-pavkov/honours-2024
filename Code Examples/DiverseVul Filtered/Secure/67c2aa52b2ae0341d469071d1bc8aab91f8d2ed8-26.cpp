static BOOL update_read_save_bitmap_order(wStream* s, const ORDER_INFO* orderInfo,
                                          SAVE_BITMAP_ORDER* save_bitmap)
{
	ORDER_FIELD_UINT32(1, save_bitmap->savedBitmapPosition);
	ORDER_FIELD_COORD(2, save_bitmap->nLeftRect);
	ORDER_FIELD_COORD(3, save_bitmap->nTopRect);
	ORDER_FIELD_COORD(4, save_bitmap->nRightRect);
	ORDER_FIELD_COORD(5, save_bitmap->nBottomRect);
	ORDER_FIELD_BYTE(6, save_bitmap->operation);
	return TRUE;
}