static int fill_buffer(multipart_buffer *self TSRMLS_DC)
{
	int bytes_to_read, total_read = 0, actual_read = 0;

	/* shift the existing data if necessary */
	if (self->bytes_in_buffer > 0 && self->buf_begin != self->buffer) {
		memmove(self->buffer, self->buf_begin, self->bytes_in_buffer);
	}

	self->buf_begin = self->buffer;

	/* calculate the free space in the buffer */
	bytes_to_read = self->bufsize - self->bytes_in_buffer;

	/* read the required number of bytes */
	while (bytes_to_read > 0) {

		char *buf = self->buffer + self->bytes_in_buffer;

		actual_read = sapi_module.read_post(buf, bytes_to_read TSRMLS_CC);

		/* update the buffer length */
		if (actual_read > 0) {
			self->bytes_in_buffer += actual_read;
			SG(read_post_bytes) += actual_read;
			total_read += actual_read;
			bytes_to_read -= actual_read;
		} else {
			break;
		}
	}

	return total_read;
}