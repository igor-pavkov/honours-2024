static av_cold int decode_init(AVCodecContext *avctx)
{
    unsigned int c;
    unsigned int channel_size;
    int num_buffers, ret;
    ALSDecContext *ctx = avctx->priv_data;
    ALSSpecificConfig *sconf = &ctx->sconf;
    ctx->avctx = avctx;

    if (!avctx->extradata) {
        av_log(avctx, AV_LOG_ERROR, "Missing required ALS extradata.\n");
        return AVERROR_INVALIDDATA;
    }

    if ((ret = read_specific_config(ctx)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "Reading ALSSpecificConfig failed.\n");
        goto fail;
    }

    if ((ret = check_specific_config(ctx)) < 0) {
        goto fail;
    }

    if (sconf->bgmc) {
        ret = ff_bgmc_init(avctx, &ctx->bgmc_lut, &ctx->bgmc_lut_status);
        if (ret < 0)
            goto fail;
    }
    if (sconf->floating) {
        avctx->sample_fmt          = AV_SAMPLE_FMT_FLT;
        avctx->bits_per_raw_sample = 32;
    } else {
        avctx->sample_fmt          = sconf->resolution > 1
                                     ? AV_SAMPLE_FMT_S32 : AV_SAMPLE_FMT_S16;
        avctx->bits_per_raw_sample = (sconf->resolution + 1) * 8;
    }

    // set maximum Rice parameter for progressive decoding based on resolution
    // This is not specified in 14496-3 but actually done by the reference
    // codec RM22 revision 2.
    ctx->s_max = sconf->resolution > 1 ? 31 : 15;

    // set lag value for long-term prediction
    ctx->ltp_lag_length = 8 + (avctx->sample_rate >=  96000) +
                              (avctx->sample_rate >= 192000);

    // allocate quantized parcor coefficient buffer
    num_buffers = sconf->mc_coding ? avctx->channels : 1;

    ctx->quant_cof        = av_malloc(sizeof(*ctx->quant_cof) * num_buffers);
    ctx->lpc_cof          = av_malloc(sizeof(*ctx->lpc_cof)   * num_buffers);
    ctx->quant_cof_buffer = av_malloc(sizeof(*ctx->quant_cof_buffer) *
                                      num_buffers * sconf->max_order);
    ctx->lpc_cof_buffer   = av_malloc(sizeof(*ctx->lpc_cof_buffer) *
                                      num_buffers * sconf->max_order);
    ctx->lpc_cof_reversed_buffer = av_malloc(sizeof(*ctx->lpc_cof_buffer) *
                                             sconf->max_order);

    if (!ctx->quant_cof              || !ctx->lpc_cof        ||
        !ctx->quant_cof_buffer       || !ctx->lpc_cof_buffer ||
        !ctx->lpc_cof_reversed_buffer) {
        av_log(avctx, AV_LOG_ERROR, "Allocating buffer memory failed.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    // assign quantized parcor coefficient buffers
    for (c = 0; c < num_buffers; c++) {
        ctx->quant_cof[c] = ctx->quant_cof_buffer + c * sconf->max_order;
        ctx->lpc_cof[c]   = ctx->lpc_cof_buffer   + c * sconf->max_order;
    }

    // allocate and assign lag and gain data buffer for ltp mode
    ctx->const_block     = av_malloc (sizeof(*ctx->const_block) * num_buffers);
    ctx->shift_lsbs      = av_malloc (sizeof(*ctx->shift_lsbs)  * num_buffers);
    ctx->opt_order       = av_malloc (sizeof(*ctx->opt_order)   * num_buffers);
    ctx->store_prev_samples = av_malloc(sizeof(*ctx->store_prev_samples) * num_buffers);
    ctx->use_ltp         = av_mallocz(sizeof(*ctx->use_ltp)  * num_buffers);
    ctx->ltp_lag         = av_malloc (sizeof(*ctx->ltp_lag)  * num_buffers);
    ctx->ltp_gain        = av_malloc (sizeof(*ctx->ltp_gain) * num_buffers);
    ctx->ltp_gain_buffer = av_malloc (sizeof(*ctx->ltp_gain_buffer) *
                                      num_buffers * 5);

    if (!ctx->const_block || !ctx->shift_lsbs ||
        !ctx->opt_order || !ctx->store_prev_samples ||
        !ctx->use_ltp  || !ctx->ltp_lag ||
        !ctx->ltp_gain || !ctx->ltp_gain_buffer) {
        av_log(avctx, AV_LOG_ERROR, "Allocating buffer memory failed.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    for (c = 0; c < num_buffers; c++)
        ctx->ltp_gain[c] = ctx->ltp_gain_buffer + c * 5;

    // allocate and assign channel data buffer for mcc mode
    if (sconf->mc_coding) {
        ctx->chan_data_buffer  = av_malloc(sizeof(*ctx->chan_data_buffer) *
                                           num_buffers * num_buffers);
        ctx->chan_data         = av_malloc(sizeof(*ctx->chan_data) *
                                           num_buffers);
        ctx->reverted_channels = av_malloc(sizeof(*ctx->reverted_channels) *
                                           num_buffers);

        if (!ctx->chan_data_buffer || !ctx->chan_data || !ctx->reverted_channels) {
            av_log(avctx, AV_LOG_ERROR, "Allocating buffer memory failed.\n");
            ret = AVERROR(ENOMEM);
            goto fail;
        }

        for (c = 0; c < num_buffers; c++)
            ctx->chan_data[c] = ctx->chan_data_buffer + c * num_buffers;
    } else {
        ctx->chan_data         = NULL;
        ctx->chan_data_buffer  = NULL;
        ctx->reverted_channels = NULL;
    }

    channel_size      = sconf->frame_length + sconf->max_order;

    ctx->prev_raw_samples = av_malloc (sizeof(*ctx->prev_raw_samples) * sconf->max_order);
    ctx->raw_buffer       = av_mallocz(sizeof(*ctx->     raw_buffer)  * avctx->channels * channel_size);
    ctx->raw_samples      = av_malloc (sizeof(*ctx->     raw_samples) * avctx->channels);

    // allocate previous raw sample buffer
    if (!ctx->prev_raw_samples || !ctx->raw_buffer|| !ctx->raw_samples) {
        av_log(avctx, AV_LOG_ERROR, "Allocating buffer memory failed.\n");
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    // assign raw samples buffers
    ctx->raw_samples[0] = ctx->raw_buffer + sconf->max_order;
    for (c = 1; c < avctx->channels; c++)
        ctx->raw_samples[c] = ctx->raw_samples[c - 1] + channel_size;

    // allocate crc buffer
    if (HAVE_BIGENDIAN != sconf->msb_first && sconf->crc_enabled &&
        (avctx->err_recognition & (AV_EF_CRCCHECK|AV_EF_CAREFUL))) {
        ctx->crc_buffer = av_malloc(sizeof(*ctx->crc_buffer) *
                                    ctx->cur_frame_length *
                                    avctx->channels *
                                    av_get_bytes_per_sample(avctx->sample_fmt));
        if (!ctx->crc_buffer) {
            av_log(avctx, AV_LOG_ERROR, "Allocating buffer memory failed.\n");
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }

    ff_dsputil_init(&ctx->dsp, avctx);

    return 0;

fail:
    decode_end(avctx);
    return ret;
}