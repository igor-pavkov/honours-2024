static int AppLayerProtoDetectPMAddSignature(AppLayerProtoDetectPMCtx *ctx, DetectContentData *cd,
                                             AppProto alproto)
{
    SCEnter();

    int ret = 0;
    AppLayerProtoDetectPMSignature *s = SCMalloc(sizeof(*s));
    if (unlikely(s == NULL))
        goto error;
    memset(s, 0, sizeof(*s));

    s->alproto = alproto;
    s->cd = cd;

    /* prepend to the list */
    s->next = ctx->head;
    ctx->head = s;

    goto end;
 error:
    ret = -1;
 end:
    SCReturnInt(ret);
}