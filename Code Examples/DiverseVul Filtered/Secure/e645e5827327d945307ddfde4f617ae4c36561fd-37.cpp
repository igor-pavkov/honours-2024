static double get_float64(RzBuffer *buffer, bool *error) {
	double ret = 0;
	if (!rz_buf_read_le64(buffer, (ut64 *)&ret)) {
		*error = true;
	}
	return ret;
}