static CACHE_COLOR_TABLE_ORDER* update_read_cache_color_table_order(rdpUpdate* update, wStream* s,
                                                                    UINT16 flags)
{
	int i;
	UINT32* colorTable;
	CACHE_COLOR_TABLE_ORDER* cache_color_table = calloc(1, sizeof(CACHE_COLOR_TABLE_ORDER));

	if (!cache_color_table)
		goto fail;

	if (Stream_GetRemainingLength(s) < 3)
		goto fail;

	Stream_Read_UINT8(s, cache_color_table->cacheIndex);    /* cacheIndex (1 byte) */
	Stream_Read_UINT16(s, cache_color_table->numberColors); /* numberColors (2 bytes) */

	if (cache_color_table->numberColors != 256)
	{
		/* This field MUST be set to 256 */
		goto fail;
	}

	if (Stream_GetRemainingLength(s) < cache_color_table->numberColors * 4)
		goto fail;

	colorTable = (UINT32*)&cache_color_table->colorTable;

	for (i = 0; i < (int)cache_color_table->numberColors; i++)
		update_read_color_quad(s, &colorTable[i]);

	return cache_color_table;
fail:
	free_cache_color_table_order(update->context, cache_color_table);
	return NULL;
}