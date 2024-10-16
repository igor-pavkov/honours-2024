static void _php_image_stream_putc(struct gdIOCtx *ctx, int c)  {
	char ch = (char) c;
	php_stream * stream = (php_stream *)ctx->data;
	TSRMLS_FETCH();
	php_stream_write(stream, &ch, 1);
}