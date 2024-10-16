static int multipart_buffer_eof(multipart_buffer *self TSRMLS_DC)
{
	if ( (self->bytes_in_buffer == 0 && fill_buffer(self TSRMLS_CC) < 1) ) {
		return 1;
	} else {
		return 0;
	}
}