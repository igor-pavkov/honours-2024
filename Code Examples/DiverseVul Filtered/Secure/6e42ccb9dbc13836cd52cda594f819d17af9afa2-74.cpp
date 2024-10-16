static int latm_decode_audio_specific_config(struct LATMContext *latmctx,
                                             GetBitContext *gb, int asclen)
{
    AACContext *ac        = &latmctx->aac_ctx;
    AVCodecContext *avctx = ac->avctx;
    MPEG4AudioConfig m4ac = { 0 };
    int config_start_bit  = get_bits_count(gb);
    int sync_extension    = 0;
    int bits_consumed, esize;

    if (asclen) {
        sync_extension = 1;
        asclen         = FFMIN(asclen, get_bits_left(gb));
    } else
        asclen         = get_bits_left(gb);

    if (config_start_bit % 8) {
        avpriv_request_sample(latmctx->aac_ctx.avctx,
                              "Non-byte-aligned audio-specific config");
        return AVERROR_PATCHWELCOME;
    }
    if (asclen <= 0)
        return AVERROR_INVALIDDATA;
    bits_consumed = decode_audio_specific_config(NULL, avctx, &m4ac,
                                         gb->buffer + (config_start_bit / 8),
                                         asclen, sync_extension);

    if (bits_consumed < 0)
        return AVERROR_INVALIDDATA;

    if (!latmctx->initialized ||
        ac->oc[1].m4ac.sample_rate != m4ac.sample_rate ||
        ac->oc[1].m4ac.chan_config != m4ac.chan_config) {

        if(latmctx->initialized) {
            av_log(avctx, AV_LOG_INFO, "audio config changed\n");
        } else {
            av_log(avctx, AV_LOG_DEBUG, "initializing latmctx\n");
        }
        latmctx->initialized = 0;

        esize = (bits_consumed+7) / 8;

        if (avctx->extradata_size < esize) {
            av_free(avctx->extradata);
            avctx->extradata = av_malloc(esize + FF_INPUT_BUFFER_PADDING_SIZE);
            if (!avctx->extradata)
                return AVERROR(ENOMEM);
        }

        avctx->extradata_size = esize;
        memcpy(avctx->extradata, gb->buffer + (config_start_bit/8), esize);
        memset(avctx->extradata+esize, 0, FF_INPUT_BUFFER_PADDING_SIZE);
    }
    skip_bits_long(gb, bits_consumed);

    return bits_consumed;
}