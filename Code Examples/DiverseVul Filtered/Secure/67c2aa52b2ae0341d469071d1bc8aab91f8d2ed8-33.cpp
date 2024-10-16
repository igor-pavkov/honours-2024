BOOL update_write_cache_bitmap_order(wStream* s, const CACHE_BITMAP_ORDER* cache_bitmap,
                                     BOOL compressed, UINT16* flags)
{
	UINT32 bitmapLength = cache_bitmap->bitmapLength;
	int inf = update_approximate_cache_bitmap_order(cache_bitmap, compressed, flags);

	if (!Stream_EnsureRemainingCapacity(s, inf))
		return FALSE;

	*flags = NO_BITMAP_COMPRESSION_HDR;

	if ((*flags & NO_BITMAP_COMPRESSION_HDR) == 0)
		bitmapLength += 8;

	Stream_Write_UINT8(s, cache_bitmap->cacheId);      /* cacheId (1 byte) */
	Stream_Write_UINT8(s, 0);                          /* pad1Octet (1 byte) */
	Stream_Write_UINT8(s, cache_bitmap->bitmapWidth);  /* bitmapWidth (1 byte) */
	Stream_Write_UINT8(s, cache_bitmap->bitmapHeight); /* bitmapHeight (1 byte) */
	Stream_Write_UINT8(s, cache_bitmap->bitmapBpp);    /* bitmapBpp (1 byte) */
	Stream_Write_UINT16(s, bitmapLength);              /* bitmapLength (2 bytes) */
	Stream_Write_UINT16(s, cache_bitmap->cacheIndex);  /* cacheIndex (2 bytes) */

	if (compressed)
	{
		if ((*flags & NO_BITMAP_COMPRESSION_HDR) == 0)
		{
			BYTE* bitmapComprHdr = (BYTE*)&(cache_bitmap->bitmapComprHdr);
			Stream_Write(s, bitmapComprHdr, 8); /* bitmapComprHdr (8 bytes) */
			bitmapLength -= 8;
		}

		Stream_Write(s, cache_bitmap->bitmapDataStream, bitmapLength);
	}
	else
	{
		Stream_Write(s, cache_bitmap->bitmapDataStream, bitmapLength);
	}

	return TRUE;
}