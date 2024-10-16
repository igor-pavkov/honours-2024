static char *get_line(multipart_buffer *self TSRMLS_DC)
{
	char* ptr = next_line(self);

	if (!ptr) {
		fill_buffer(self TSRMLS_CC);
		ptr = next_line(self);
	}

	return ptr;
}