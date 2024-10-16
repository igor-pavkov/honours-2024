static void _php_image_output_putc(struct gdIOCtx *ctx, int c)
{
	/* without the following downcast, the write will fail
	 * (i.e., will write a zero byte) for all
	 * big endian architectures:
	 */
	unsigned char ch = (unsigned char) c;
	TSRMLS_FETCH();
	php_write(&ch, 1 TSRMLS_CC);
}