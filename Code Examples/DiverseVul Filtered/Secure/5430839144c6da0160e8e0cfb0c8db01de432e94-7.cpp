static int cmv_decode_frame(AVCodecContext *avctx,
                            void *data, int *got_frame,
                            AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    CmvContext *s = avctx->priv_data;
    const uint8_t *buf_end = buf + buf_size;
    AVFrame *frame = data;
    int ret;

    if (buf_end - buf < EA_PREAMBLE_SIZE)
        return AVERROR_INVALIDDATA;

    if (AV_RL32(buf)==MVIh_TAG||AV_RB32(buf)==MVIh_TAG) {
        ret = cmv_process_header(s, buf+EA_PREAMBLE_SIZE, buf_end);
        if (ret < 0)
            return ret;
        return buf_size;
    }

    if (av_image_check_size(s->width, s->height, 0, s->avctx))
        return -1;

    if ((ret = ff_get_buffer(avctx, frame, AV_GET_BUFFER_FLAG_REF)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }

    memcpy(frame->data[1], s->palette, AVPALETTE_SIZE);

    buf += EA_PREAMBLE_SIZE;
    if ((buf[0]&1)) {  // subtype
        cmv_decode_inter(s, frame, buf+2, buf_end);
        frame->key_frame = 0;
        frame->pict_type = AV_PICTURE_TYPE_P;
    }else{
        frame->key_frame = 1;
        frame->pict_type = AV_PICTURE_TYPE_I;
        cmv_decode_intra(s, frame, buf+2, buf_end);
    }

    av_frame_unref(s->last2_frame);
    av_frame_move_ref(s->last2_frame, s->last_frame);
    if ((ret = av_frame_ref(s->last_frame, frame)) < 0)
        return ret;

    *got_frame = 1;

    return buf_size;
}