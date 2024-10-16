static BOOL update_read_cached_icon_info(wStream* s, CACHED_ICON_INFO* cachedIconInfo)
{
	if (Stream_GetRemainingLength(s) < 3)
		return FALSE;

	Stream_Read_UINT16(s, cachedIconInfo->cacheEntry); /* cacheEntry (2 bytes) */
	Stream_Read_UINT8(s, cachedIconInfo->cacheId);     /* cacheId (1 byte) */
	return TRUE;
}