static char *__read_nonnull_str_at(RBuffer *buf, ut64 *offset) {
	ut8 size = r_buf_read8_at (buf, *offset);
	size &= 0x7F; // Max is 127
	if (!size) {
		return NULL;
	}
	(*offset)++;
	char *str = calloc ((ut64)size + 1, sizeof (char));
	r_buf_read_at (buf, *offset, (ut8 *)str, size);
	*offset += size;
	return str;
}