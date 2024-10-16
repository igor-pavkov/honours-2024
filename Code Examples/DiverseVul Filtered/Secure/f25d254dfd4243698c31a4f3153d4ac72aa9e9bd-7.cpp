static int top_up(STATE_PARAM unsigned n)
{
	int count = bytebuffer_size - bytebuffer_offset;

	if (count < (int)n) {
		memmove(bytebuffer, &bytebuffer[bytebuffer_offset], count);
		bytebuffer_offset = 0;
		bytebuffer_size = full_read(gunzip_src_fd, &bytebuffer[count], bytebuffer_max - count);
		if ((int)bytebuffer_size < 0) {
			bb_simple_error_msg(bb_msg_read_error);
			return 0;
		}
		bytebuffer_size += count;
		if (bytebuffer_size < n)
			return 0;
	}
	return 1;
}