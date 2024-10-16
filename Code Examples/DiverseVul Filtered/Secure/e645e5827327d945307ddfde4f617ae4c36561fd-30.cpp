static st32 get_st32(RzBuffer *buffer, bool *error) {
	st32 ret = 0;
	if (!rz_buf_read_le32(buffer, (ut32 *)&ret)) {
		*error = true;
	}
	return ret;
}