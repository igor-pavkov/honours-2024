static int AppLayerProtoDetectPMSetContentIDs(AppLayerProtoDetectPMCtx *ctx)
{
    SCEnter();

    typedef struct TempContainer_ {
        PatIntId id;
        uint16_t content_len;
        uint8_t *content;
    } TempContainer;

    AppLayerProtoDetectPMSignature *s = NULL;
    uint32_t struct_total_size = 0;
    uint32_t content_total_size = 0;
    /* array hash buffer */
    uint8_t *ahb = NULL;
    uint8_t *content = NULL;
    uint8_t content_len = 0;
    PatIntId max_id = 0;
    TempContainer *struct_offset = NULL;
    uint8_t *content_offset = NULL;
    int ret = 0;

    if (ctx->head == NULL)
        goto end;

    for (s = ctx->head; s != NULL; s = s->next) {
        struct_total_size += sizeof(TempContainer);
        content_total_size += s->cd->content_len;
        ctx->max_sig_id++;
    }

    ahb = SCMalloc(sizeof(uint8_t) * (struct_total_size + content_total_size));
    if (unlikely(ahb == NULL))
        goto error;

    struct_offset = (TempContainer *)ahb;
    content_offset = ahb + struct_total_size;
    for (s = ctx->head; s != NULL; s = s->next) {
        TempContainer *tcdup = (TempContainer *)ahb;
        content = s->cd->content;
        content_len = s->cd->content_len;

        for (; tcdup != struct_offset; tcdup++) {
            if (tcdup->content_len != content_len ||
                SCMemcmp(tcdup->content, content, tcdup->content_len) != 0)
            {
                continue;
            }
            break;
        }

        if (tcdup != struct_offset) {
            s->cd->id = tcdup->id;
            continue;
        }

        struct_offset->content_len = content_len;
        struct_offset->content = content_offset;
        content_offset += content_len;
        memcpy(struct_offset->content, content, content_len);
        struct_offset->id = max_id++;
        s->cd->id = struct_offset->id;

        struct_offset++;
    }

    ctx->max_pat_id = max_id;

    goto end;
 error:
    ret = -1;
 end:
    if (ahb != NULL)
        SCFree(ahb);
    SCReturnInt(ret);
}