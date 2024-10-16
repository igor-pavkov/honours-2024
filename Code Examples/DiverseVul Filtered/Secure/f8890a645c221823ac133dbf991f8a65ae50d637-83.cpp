static void update_write_refresh_rect(wStream* s, BYTE count, const RECTANGLE_16* areas)
{
	int i;
	Stream_Write_UINT8(s, count); /* numberOfAreas (1 byte) */
	Stream_Seek(s, 3);            /* pad3Octets (3 bytes) */

	for (i = 0; i < count; i++)
	{
		Stream_Write_UINT16(s, areas[i].left);   /* left (2 bytes) */
		Stream_Write_UINT16(s, areas[i].top);    /* top (2 bytes) */
		Stream_Write_UINT16(s, areas[i].right);  /* right (2 bytes) */
		Stream_Write_UINT16(s, areas[i].bottom); /* bottom (2 bytes) */
	}
}