int update_approximate_cache_bitmap_v3_order(CACHE_BITMAP_V3_ORDER* cache_bitmap_v3, UINT16* flags)
{
	BITMAP_DATA_EX* bitmapData = &cache_bitmap_v3->bitmapData;
	return 64 + bitmapData->length;
}