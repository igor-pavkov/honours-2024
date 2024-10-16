static multipart_buffer *multipart_buffer_new(char *boundary, int boundary_len TSRMLS_DC)
{
	multipart_buffer *self = (multipart_buffer *) ecalloc(1, sizeof(multipart_buffer));

	int minsize = boundary_len + 6;
	if (minsize < FILLUNIT) minsize = FILLUNIT;

	self->buffer = (char *) ecalloc(1, minsize + 1);
	self->bufsize = minsize;

	spprintf(&self->boundary, 0, "--%s", boundary);

	self->boundary_next_len = spprintf(&self->boundary_next, 0, "\n--%s", boundary);

	self->buf_begin = self->buffer;
	self->bytes_in_buffer = 0;

	if (php_rfc1867_encoding_translation(TSRMLS_C)) {
		php_rfc1867_get_detect_order(&self->detect_order, &self->detect_order_size TSRMLS_CC);
	} else {
		self->detect_order = NULL;
		self->detect_order_size = 0;
	}

	self->input_encoding = NULL;

	return self;
}