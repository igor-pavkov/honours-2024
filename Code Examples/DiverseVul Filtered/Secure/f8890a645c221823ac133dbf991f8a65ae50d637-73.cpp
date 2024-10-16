static void update_write_suppress_output(wStream* s, BYTE allow, const RECTANGLE_16* area)
{
	Stream_Write_UINT8(s, allow); /* allowDisplayUpdates (1 byte) */
	/* Use zeros for padding (like mstsc) for compatibility with legacy servers */
	Stream_Zero(s, 3); /* pad3Octets (3 bytes) */

	if (allow > 0)
	{
		Stream_Write_UINT16(s, area->left);   /* left (2 bytes) */
		Stream_Write_UINT16(s, area->top);    /* top (2 bytes) */
		Stream_Write_UINT16(s, area->right);  /* right (2 bytes) */
		Stream_Write_UINT16(s, area->bottom); /* bottom (2 bytes) */
	}
}