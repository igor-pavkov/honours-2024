static int AppLayerProtoDetectPMPrepareMpm(AppLayerProtoDetectPMCtx *ctx)
{
    SCEnter();

    int ret = 0;
    MpmCtx *mpm_ctx = &ctx->mpm_ctx;

    if (mpm_table[mpm_ctx->mpm_type].Prepare(mpm_ctx) < 0)
        goto error;

    goto end;
 error:
    ret = -1;
 end:
    SCReturnInt(ret);
}