UINT rdpgfx_write_rect16(wStream* s, const RECTANGLE_16* rect16)
{
	Stream_Write_UINT16(s, rect16->left);   /* left (2 bytes) */
	Stream_Write_UINT16(s, rect16->top);    /* top (2 bytes) */
	Stream_Write_UINT16(s, rect16->right);  /* right (2 bytes) */
	Stream_Write_UINT16(s, rect16->bottom); /* bottom (2 bytes) */
	return CHANNEL_RC_OK;
}