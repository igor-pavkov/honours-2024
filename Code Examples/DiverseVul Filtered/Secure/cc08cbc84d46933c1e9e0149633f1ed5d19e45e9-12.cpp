gdIOCtx * gdNewDynamicCtxEx (int initialSize, void *data, int freeOKFlag)
{
	dpIOCtx *ctx;
	dynamicPtr *dp;

	ctx = (dpIOCtx *) gdMalloc (sizeof (dpIOCtx));

	dp = newDynamic(initialSize, data, freeOKFlag);

	ctx->dp = dp;

	ctx->ctx.getC = dynamicGetchar;
	ctx->ctx.putC = dynamicPutchar;

	ctx->ctx.getBuf = dynamicGetbuf;
	ctx->ctx.putBuf = dynamicPutbuf;

	ctx->ctx.seek = dynamicSeek;
	ctx->ctx.tell = dynamicTell;

	ctx->ctx.gd_free = gdFreeDynamicCtx;

	return (gdIOCtx *) ctx;
}