BOOL update_write_cache_color_table_order(wStream* s,
                                          const CACHE_COLOR_TABLE_ORDER* cache_color_table,
                                          UINT16* flags)
{
	int i, inf;
	UINT32* colorTable;

	if (cache_color_table->numberColors != 256)
		return FALSE;

	inf = update_approximate_cache_color_table_order(cache_color_table, flags);

	if (!Stream_EnsureRemainingCapacity(s, inf))
		return FALSE;

	Stream_Write_UINT8(s, cache_color_table->cacheIndex);    /* cacheIndex (1 byte) */
	Stream_Write_UINT16(s, cache_color_table->numberColors); /* numberColors (2 bytes) */
	colorTable = (UINT32*)&cache_color_table->colorTable;

	for (i = 0; i < (int)cache_color_table->numberColors; i++)
	{
		update_write_color_quad(s, colorTable[i]);
	}

	return TRUE;
}