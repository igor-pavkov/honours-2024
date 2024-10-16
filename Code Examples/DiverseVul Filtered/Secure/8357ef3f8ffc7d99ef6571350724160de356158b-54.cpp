static AppProto AppLayerProtoDetectPMMatchSignature(const AppLayerProtoDetectPMSignature *s,
                                                    AppLayerProtoDetectThreadCtx *tctx,
                                                    uint8_t *buf, uint16_t buflen,
                                                    uint8_t ipproto)
{
    SCEnter();
    AppProto proto = ALPROTO_UNKNOWN;
    uint8_t *found = NULL;

    if (s->cd->offset > buflen) {
        SCLogDebug("s->co->offset (%"PRIu16") > buflen (%"PRIu16")",
                   s->cd->offset, buflen);
        goto end;
    }

    if (s->cd->depth > buflen) {
        SCLogDebug("s->co->depth (%"PRIu16") > buflen (%"PRIu16")",
                   s->cd->depth, buflen);
        goto end;
    }

    uint8_t *sbuf = buf + s->cd->offset;
    uint16_t sbuflen = s->cd->depth - s->cd->offset;
    SCLogDebug("s->co->offset (%"PRIu16") s->cd->depth (%"PRIu16")",
               s->cd->offset, s->cd->depth);

    found = SpmScan(s->cd->spm_ctx, tctx->spm_thread_ctx, sbuf, sbuflen);
    if (found != NULL)
        proto = s->alproto;

 end:
    SCReturnUInt(proto);
}