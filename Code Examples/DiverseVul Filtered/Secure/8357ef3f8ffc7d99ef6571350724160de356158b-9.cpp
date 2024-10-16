static int AppLayerProtoDetectPMMapSignatures(AppLayerProtoDetectPMCtx *ctx)
{
    SCEnter();

    int ret = 0;
    AppLayerProtoDetectPMSignature *s, *next_s;
    int mpm_ret;
    SigIntId id = 0;

    ctx->map = SCMalloc(ctx->max_sig_id * sizeof(AppLayerProtoDetectPMSignature *));
    if (ctx->map == NULL)
        goto error;
    memset(ctx->map, 0, ctx->max_sig_id * sizeof(AppLayerProtoDetectPMSignature *));

    /* add an array indexed by rule id to look up the sig */
    for (s = ctx->head; s != NULL; ) {
        next_s = s->next;
        s->id = id++;
        SCLogDebug("s->id %u", s->id);

        if (s->cd->flags & DETECT_CONTENT_NOCASE) {
            mpm_ret = MpmAddPatternCI(&ctx->mpm_ctx,
                                      s->cd->content, s->cd->content_len,
                                      0, 0, s->cd->id, s->id, 0);
            if (mpm_ret < 0)
                goto error;
        } else {
            mpm_ret = MpmAddPatternCS(&ctx->mpm_ctx,
                                      s->cd->content, s->cd->content_len,
                                      0, 0, s->cd->id, s->id, 0);
            if (mpm_ret < 0)
                goto error;
        }

        ctx->map[s->id] = s;
        s->next = NULL;
        s = next_s;
    }
    ctx->head = NULL;

    goto end;
 error:
    ret = -1;
 end:
    SCReturnInt(ret);
}