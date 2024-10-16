int update_approximate_cache_bitmap_order(const CACHE_BITMAP_ORDER* cache_bitmap, BOOL compressed,
                                          UINT16* flags)
{
	return 64 + cache_bitmap->bitmapLength;
}