UINT rdpgfx_read_point16(wStream* s, RDPGFX_POINT16* pt16)
{
	if (Stream_GetRemainingLength(s) < 4)
	{
		WLog_ERR(TAG, "not enough data!");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT16(s, pt16->x); /* x (2 bytes) */
	Stream_Read_UINT16(s, pt16->y); /* y (2 bytes) */
	return CHANNEL_RC_OK;
}