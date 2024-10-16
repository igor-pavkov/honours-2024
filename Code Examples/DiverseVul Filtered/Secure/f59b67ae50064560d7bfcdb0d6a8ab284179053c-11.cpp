static inline size_t phar_stream_copy_to_stream(php_stream *src, php_stream *dest, size_t maxlen, size_t *len STREAMS_DC TSRMLS_DC)
{
	size_t ret = php_stream_copy_to_stream(src, dest, maxlen);
	if (len) {
		*len = ret;
	}
	if (ret) {
		return SUCCESS;
	}
	return FAILURE;
}