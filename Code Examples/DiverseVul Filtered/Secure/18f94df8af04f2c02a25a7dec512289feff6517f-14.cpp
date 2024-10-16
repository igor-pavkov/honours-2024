static int decode_frame(AVCodecContext *avctx, void *data, int *got_frame_ptr,
                        AVPacket *avpkt)
{
    ALSDecContext *ctx       = avctx->priv_data;
    AVFrame *frame           = data;
    ALSSpecificConfig *sconf = &ctx->sconf;
    const uint8_t *buffer    = avpkt->data;
    int buffer_size          = avpkt->size;
    int invalid_frame, ret;
    unsigned int c, sample, ra_frame, bytes_read, shift;

    init_get_bits(&ctx->gb, buffer, buffer_size * 8);

    // In the case that the distance between random access frames is set to zero
    // (sconf->ra_distance == 0) no frame is treated as a random access frame.
    // For the first frame, if prediction is used, all samples used from the
    // previous frame are assumed to be zero.
    ra_frame = sconf->ra_distance && !(ctx->frame_id % sconf->ra_distance);

    // the last frame to decode might have a different length
    if (sconf->samples != 0xFFFFFFFF)
        ctx->cur_frame_length = FFMIN(sconf->samples - ctx->frame_id * (uint64_t) sconf->frame_length,
                                      sconf->frame_length);
    else
        ctx->cur_frame_length = sconf->frame_length;

    // decode the frame data
    if ((invalid_frame = read_frame_data(ctx, ra_frame)) < 0)
        av_log(ctx->avctx, AV_LOG_WARNING,
               "Reading frame data failed. Skipping RA unit.\n");

    ctx->frame_id++;

    /* get output buffer */
    frame->nb_samples = ctx->cur_frame_length;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;

    // transform decoded frame into output format
    #define INTERLEAVE_OUTPUT(bps)                                                   \
    {                                                                                \
        int##bps##_t *dest = (int##bps##_t*)frame->data[0];                          \
        shift = bps - ctx->avctx->bits_per_raw_sample;                               \
        if (!ctx->cs_switch) {                                                       \
            for (sample = 0; sample < ctx->cur_frame_length; sample++)               \
                for (c = 0; c < avctx->channels; c++)                                \
                    *dest++ = ctx->raw_samples[c][sample] << shift;                  \
        } else {                                                                     \
            for (sample = 0; sample < ctx->cur_frame_length; sample++)               \
                for (c = 0; c < avctx->channels; c++)                                \
                    *dest++ = ctx->raw_samples[sconf->chan_pos[c]][sample] << shift; \
        }                                                                            \
    }

    if (ctx->avctx->bits_per_raw_sample <= 16) {
        INTERLEAVE_OUTPUT(16)
    } else {
        INTERLEAVE_OUTPUT(32)
    }

    // update CRC
    if (sconf->crc_enabled && (avctx->err_recognition & (AV_EF_CRCCHECK|AV_EF_CAREFUL))) {
        int swap = HAVE_BIGENDIAN != sconf->msb_first;

        if (ctx->avctx->bits_per_raw_sample == 24) {
            int32_t *src = (int32_t *)frame->data[0];

            for (sample = 0;
                 sample < ctx->cur_frame_length * avctx->channels;
                 sample++) {
                int32_t v;

                if (swap)
                    v = av_bswap32(src[sample]);
                else
                    v = src[sample];
                if (!HAVE_BIGENDIAN)
                    v >>= 8;

                ctx->crc = av_crc(ctx->crc_table, ctx->crc, (uint8_t*)(&v), 3);
            }
        } else {
            uint8_t *crc_source;

            if (swap) {
                if (ctx->avctx->bits_per_raw_sample <= 16) {
                    int16_t *src  = (int16_t*) frame->data[0];
                    int16_t *dest = (int16_t*) ctx->crc_buffer;
                    for (sample = 0;
                         sample < ctx->cur_frame_length * avctx->channels;
                         sample++)
                        *dest++ = av_bswap16(src[sample]);
                } else {
                    ctx->dsp.bswap_buf((uint32_t*)ctx->crc_buffer,
                                       (uint32_t *)frame->data[0],
                                       ctx->cur_frame_length * avctx->channels);
                }
                crc_source = ctx->crc_buffer;
            } else {
                crc_source = frame->data[0];
            }

            ctx->crc = av_crc(ctx->crc_table, ctx->crc, crc_source,
                              ctx->cur_frame_length * avctx->channels *
                              av_get_bytes_per_sample(avctx->sample_fmt));
        }


        // check CRC sums if this is the last frame
        if (ctx->cur_frame_length != sconf->frame_length &&
            ctx->crc_org != ctx->crc) {
            av_log(avctx, AV_LOG_ERROR, "CRC error.\n");
            if (avctx->err_recognition & AV_EF_EXPLODE)
                return AVERROR_INVALIDDATA;
        }
    }

    *got_frame_ptr = 1;

    bytes_read = invalid_frame ? buffer_size :
                                 (get_bits_count(&ctx->gb) + 7) >> 3;

    return bytes_read;
}