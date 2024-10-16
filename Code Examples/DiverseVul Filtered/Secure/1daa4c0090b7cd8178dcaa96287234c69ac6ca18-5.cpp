static int _php_image_output_putbuf(struct gdIOCtx *ctx, const void* buf, int l)
{
	TSRMLS_FETCH();
	return php_write((void *)buf, l TSRMLS_CC);
}