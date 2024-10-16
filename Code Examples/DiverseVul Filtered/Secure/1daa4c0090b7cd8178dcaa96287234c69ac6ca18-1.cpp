static void _php_image_stream_ctxfree(struct gdIOCtx *ctx)
{
	TSRMLS_FETCH();

	if(ctx->data) {
		php_stream_close((php_stream *) ctx->data);
		ctx->data = NULL;
	}
	if(ctx) {
		efree(ctx);
	}
}