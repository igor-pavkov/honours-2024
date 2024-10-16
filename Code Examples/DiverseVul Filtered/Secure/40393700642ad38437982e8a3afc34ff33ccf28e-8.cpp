UINT rdpgfx_write_header(wStream* s, const RDPGFX_HEADER* header)
{
	Stream_Write_UINT16(s, header->cmdId);     /* cmdId (2 bytes) */
	Stream_Write_UINT16(s, header->flags);     /* flags (2 bytes) */
	Stream_Write_UINT32(s, header->pduLength); /* pduLength (4 bytes) */
	return CHANNEL_RC_OK;
}