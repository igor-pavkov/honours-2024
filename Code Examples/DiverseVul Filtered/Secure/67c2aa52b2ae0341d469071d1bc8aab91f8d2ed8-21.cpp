int update_approximate_cache_bitmap_v2_order(CACHE_BITMAP_V2_ORDER* cache_bitmap_v2,
                                             BOOL compressed, UINT16* flags)
{
	return 64 + cache_bitmap_v2->bitmapLength;
}