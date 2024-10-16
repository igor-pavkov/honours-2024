static ut8 *get_bytes(RzBuffer *buffer, ut32 size) {
	ut8 *ret = RZ_NEWS0(ut8, size + 1);
	if (!ret) {
		return NULL;
	}
	if (rz_buf_read(buffer, ret, size) < size) {
		free(ret);
		return NULL;
	}
	return ret;
}