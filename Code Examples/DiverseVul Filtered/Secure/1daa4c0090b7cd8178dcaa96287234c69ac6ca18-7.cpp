static int _php_image_stream_putbuf(struct gdIOCtx *ctx, const void* buf, int l)
{
	php_stream * stream = (php_stream *)ctx->data;
	TSRMLS_FETCH();
	return php_stream_write(stream, (void *)buf, l);
}