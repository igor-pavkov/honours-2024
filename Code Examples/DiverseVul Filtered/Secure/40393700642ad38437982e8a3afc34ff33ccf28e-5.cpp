UINT rdpgfx_write_color32(wStream* s, const RDPGFX_COLOR32* color32)
{
	Stream_Write_UINT8(s, color32->B);  /* B (1 byte) */
	Stream_Write_UINT8(s, color32->G);  /* G (1 byte) */
	Stream_Write_UINT8(s, color32->R);  /* R (1 byte) */
	Stream_Write_UINT8(s, color32->XA); /* XA (1 byte) */
	return CHANNEL_RC_OK;
}