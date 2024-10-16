u8 flac_dmx_crc8(u8 *data, u32 len)
{
	u8 crc = 0;
	while (len--)
		crc = flac_dmx_crc8_table[crc ^ *data++];
	return crc;
}