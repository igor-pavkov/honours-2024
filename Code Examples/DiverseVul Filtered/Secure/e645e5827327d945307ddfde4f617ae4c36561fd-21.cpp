static st64 get_st64(RzBuffer *buffer, bool *error) {
	st64 ret = 0;
	if (!rz_buf_read_le64(buffer, (ut64 *)&ret)) {
		*error = true;
	}
	return ret;
}