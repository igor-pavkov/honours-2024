static ut8 get_ut8(RzBuffer *buffer, bool *error) {
	ut8 ret = 0;
	int size = rz_buf_read(buffer, &ret, sizeof(ret));
	if (size < sizeof(ret)) {
		*error = true;
	}
	return ret;
}