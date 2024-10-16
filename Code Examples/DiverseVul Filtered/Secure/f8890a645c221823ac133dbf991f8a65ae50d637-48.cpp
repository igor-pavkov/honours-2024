PALETTE_UPDATE* update_read_palette(rdpUpdate* update, wStream* s)
{
	int i;
	PALETTE_ENTRY* entry;
	PALETTE_UPDATE* palette_update = calloc(1, sizeof(PALETTE_UPDATE));

	if (!palette_update)
		goto fail;

	if (Stream_GetRemainingLength(s) < 6)
		goto fail;

	Stream_Seek_UINT16(s);                         /* pad2Octets (2 bytes) */
	Stream_Read_UINT32(s, palette_update->number); /* numberColors (4 bytes), must be set to 256 */

	if (palette_update->number > 256)
		palette_update->number = 256;

	if (Stream_GetRemainingLength(s) < palette_update->number * 3)
		goto fail;

	/* paletteEntries */
	for (i = 0; i < (int)palette_update->number; i++)
	{
		entry = &palette_update->entries[i];
		Stream_Read_UINT8(s, entry->red);
		Stream_Read_UINT8(s, entry->green);
		Stream_Read_UINT8(s, entry->blue);
	}

	return palette_update;
fail:
	free_palette_update(update->context, palette_update);
	return NULL;
}