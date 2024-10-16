static ut16 get_ut16(RzBuffer *buffer, bool *error) {
	ut16 ret = 0;

	if (!rz_buf_read_le16(buffer, &ret)) {
		*error = true;
	}
	return ret;
}