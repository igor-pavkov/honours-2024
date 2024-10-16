UINT rdpgfx_read_color32(wStream* s, RDPGFX_COLOR32* color32)
{
	if (Stream_GetRemainingLength(s) < 4)
	{
		WLog_ERR(TAG, "not enough data!");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT8(s, color32->B);  /* B (1 byte) */
	Stream_Read_UINT8(s, color32->G);  /* G (1 byte) */
	Stream_Read_UINT8(s, color32->R);  /* R (1 byte) */
	Stream_Read_UINT8(s, color32->XA); /* XA (1 byte) */
	return CHANNEL_RC_OK;
}