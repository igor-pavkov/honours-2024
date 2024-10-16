static int init_offset(ShortenContext *s)
{
    int32_t mean = 0;
    int chan, i;
    int nblock = FFMAX(1, s->nmean);
    /* initialise offset */
    switch (s->internal_ftype) {
    case TYPE_S16HL:
    case TYPE_S16LH:
        mean = 0;
        break;
    default:
        av_log(s->avctx, AV_LOG_ERROR, "unknown audio type");
        return AVERROR_INVALIDDATA;
    }

    for (chan = 0; chan < s->channels; chan++)
        for (i = 0; i < nblock; i++)
            s->offset[chan][i] = mean;
    return 0;
}