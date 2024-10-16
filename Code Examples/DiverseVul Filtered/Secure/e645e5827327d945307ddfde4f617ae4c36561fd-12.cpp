static ut32 get_ut32(RzBuffer *buffer, bool *error) {
	ut32 ret = 0;
	if (!rz_buf_read_le32(buffer, &ret)) {
		*error = true;
	}
	return ret;
}