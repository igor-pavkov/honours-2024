static void gdFreeDynamicCtx (struct gdIOCtx *ctx)
{
	dynamicPtr *dp;
	dpIOCtx *dctx;

	dctx = (dpIOCtx *) ctx;
	dp = dctx->dp;

	gdFree(ctx);

	dp->realSize = 0;
	dp->logicalSize = 0;

	gdFree(dp);
}