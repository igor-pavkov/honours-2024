static unsigned fill_bitbuffer(STATE_PARAM unsigned bitbuffer, unsigned *current, const unsigned required)
{
	while (*current < required) {
		if (bytebuffer_offset >= bytebuffer_size) {
			unsigned sz = bytebuffer_max - 4;
			if (to_read >= 0 && to_read < sz) /* unzip only */
				sz = to_read;
			/* Leave the first 4 bytes empty so we can always unwind the bitbuffer
			 * to the front of the bytebuffer */
			bytebuffer_size = safe_read(gunzip_src_fd, &bytebuffer[4], sz);
			if ((int)bytebuffer_size < 1) {
				error_msg = "unexpected end of file";
				abort_unzip(PASS_STATE_ONLY);
			}
			if (to_read >= 0) /* unzip only */
				to_read -= bytebuffer_size;
			bytebuffer_size += 4;
			bytebuffer_offset = 4;
		}
		bitbuffer |= ((unsigned) bytebuffer[bytebuffer_offset]) << *current;
		bytebuffer_offset++;
		*current += 8;
	}
	return bitbuffer;
}