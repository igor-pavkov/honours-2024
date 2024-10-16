static void calculate_gunzip_crc(STATE_PARAM_ONLY)
{
	gunzip_crc = crc32_block_endian0(gunzip_crc, gunzip_window, gunzip_outbuf_count, gunzip_crc_table);
	gunzip_bytes_out += gunzip_outbuf_count;
}