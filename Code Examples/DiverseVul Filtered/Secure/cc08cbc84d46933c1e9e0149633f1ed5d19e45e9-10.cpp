static void dynamicPutchar (struct gdIOCtx *ctx, int a)
{
	unsigned char b;
	dpIOCtxPtr dctx;

	b = a;
	dctx = (dpIOCtxPtr) ctx;

	appendDynamic(dctx->dp, &b, 1);
}