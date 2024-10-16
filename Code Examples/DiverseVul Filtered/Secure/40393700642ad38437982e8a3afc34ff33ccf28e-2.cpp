UINT rdpgfx_read_header(wStream* s, RDPGFX_HEADER* header)
{
	if (Stream_GetRemainingLength(s) < 8)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	Stream_Read_UINT16(s, header->cmdId);     /* cmdId (2 bytes) */
	Stream_Read_UINT16(s, header->flags);     /* flags (2 bytes) */
	Stream_Read_UINT32(s, header->pduLength); /* pduLength (4 bytes) */
	return CHANNEL_RC_OK;
}