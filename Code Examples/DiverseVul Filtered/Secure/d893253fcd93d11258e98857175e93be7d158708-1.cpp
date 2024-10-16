static int decode_frame(AVCodecContext *avctx, void *data, int *got_frame, AVPacket *avpkt)
{
    const uint8_t *buf  = avpkt->data;
    int buf_size        = avpkt->size;
    FFV1Context *f      = avctx->priv_data;
    RangeCoder *const c = &f->slice_context[0]->c;
    int i, ret;
    uint8_t keystate = 128;
    const uint8_t *buf_p;
    AVFrame *p;

    if (f->last_picture.f)
        ff_thread_release_buffer(avctx, &f->last_picture);
    FFSWAP(ThreadFrame, f->picture, f->last_picture);

    f->cur = p = f->picture.f;

    if (f->version < 3 && avctx->field_order > AV_FIELD_PROGRESSIVE) {
        /* we have interlaced material flagged in container */
        p->interlaced_frame = 1;
        if (avctx->field_order == AV_FIELD_TT || avctx->field_order == AV_FIELD_TB)
            p->top_field_first = 1;
    }

    f->avctx = avctx;
    ff_init_range_decoder(c, buf, buf_size);
    ff_build_rac_states(c, 0.05 * (1LL << 32), 256 - 8);

    p->pict_type = AV_PICTURE_TYPE_I; //FIXME I vs. P
    if (get_rac(c, &keystate)) {
        p->key_frame    = 1;
        f->key_frame_ok = 0;
        if ((ret = read_header(f)) < 0)
            return ret;
        f->key_frame_ok = 1;
    } else {
        if (!f->key_frame_ok) {
            av_log(avctx, AV_LOG_ERROR,
                   "Cannot decode non-keyframe without valid keyframe\n");
            return AVERROR_INVALIDDATA;
        }
        p->key_frame = 0;
    }

    if ((ret = ff_thread_get_buffer(avctx, &f->picture, AV_GET_BUFFER_FLAG_REF)) < 0)
        return ret;

    if (avctx->debug & FF_DEBUG_PICT_INFO)
        av_log(avctx, AV_LOG_DEBUG, "ver:%d keyframe:%d coder:%d ec:%d slices:%d bps:%d\n",
               f->version, p->key_frame, f->ac, f->ec, f->slice_count, f->avctx->bits_per_raw_sample);

    ff_thread_finish_setup(avctx);

    buf_p = buf + buf_size;
    for (i = f->slice_count - 1; i >= 0; i--) {
        FFV1Context *fs = f->slice_context[i];
        int trailer = 3 + 5*!!f->ec;
        int v;

        if (i || f->version > 2) v = AV_RB24(buf_p-trailer) + trailer;
        else                     v = buf_p - c->bytestream_start;
        if (buf_p - c->bytestream_start < v) {
            av_log(avctx, AV_LOG_ERROR, "Slice pointer chain broken\n");
            ff_thread_report_progress(&f->picture, INT_MAX, 0);
            return AVERROR_INVALIDDATA;
        }
        buf_p -= v;

        if (f->ec) {
            unsigned crc = av_crc(av_crc_get_table(AV_CRC_32_IEEE), 0, buf_p, v);
            if (crc) {
                int64_t ts = avpkt->pts != AV_NOPTS_VALUE ? avpkt->pts : avpkt->dts;
                av_log(f->avctx, AV_LOG_ERROR, "CRC mismatch %X!", crc);
                if (ts != AV_NOPTS_VALUE && avctx->pkt_timebase.num) {
                    av_log(f->avctx, AV_LOG_ERROR, "at %f seconds\n", ts*av_q2d(avctx->pkt_timebase));
                } else if (ts != AV_NOPTS_VALUE) {
                    av_log(f->avctx, AV_LOG_ERROR, "at %"PRId64"\n", ts);
                } else {
                    av_log(f->avctx, AV_LOG_ERROR, "\n");
                }
                fs->slice_damaged = 1;
            }
        }

        if (i) {
            ff_init_range_decoder(&fs->c, buf_p, v);
        } else
            fs->c.bytestream_end = (uint8_t *)(buf_p + v);

        fs->avctx = avctx;
        fs->cur = p;
    }

    avctx->execute(avctx,
                   decode_slice,
                   &f->slice_context[0],
                   NULL,
                   f->slice_count,
                   sizeof(void*));

    for (i = f->slice_count - 1; i >= 0; i--) {
        FFV1Context *fs = f->slice_context[i];
        int j;
        if (fs->slice_damaged && f->last_picture.f->data[0]) {
            const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(avctx->pix_fmt);
            const uint8_t *src[4];
            uint8_t *dst[4];
            ff_thread_await_progress(&f->last_picture, INT_MAX, 0);
            for (j = 0; j < desc->nb_components; j++) {
                int sh = (j == 1 || j == 2) ? f->chroma_h_shift : 0;
                int sv = (j == 1 || j == 2) ? f->chroma_v_shift : 0;
                dst[j] = p->data[j] + p->linesize[j] *
                         (fs->slice_y >> sv) + (fs->slice_x >> sh);
                src[j] = f->last_picture.f->data[j] + f->last_picture.f->linesize[j] *
                         (fs->slice_y >> sv) + (fs->slice_x >> sh);

            }
            if (desc->flags & AV_PIX_FMT_FLAG_PAL ||
                desc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL) {
                dst[1] = p->data[1];
                src[1] = f->last_picture.f->data[1];
            }
            av_image_copy(dst, p->linesize, (const uint8_t **)src,
                          f->last_picture.f->linesize,
                          avctx->pix_fmt,
                          fs->slice_width,
                          fs->slice_height);
        }
    }
    ff_thread_report_progress(&f->picture, INT_MAX, 0);

    f->picture_number++;

    if (f->last_picture.f)
        ff_thread_release_buffer(avctx, &f->last_picture);
    f->cur = NULL;
    if ((ret = av_frame_ref(data, f->picture.f)) < 0)
        return ret;

    *got_frame = 1;

    return buf_size;
}