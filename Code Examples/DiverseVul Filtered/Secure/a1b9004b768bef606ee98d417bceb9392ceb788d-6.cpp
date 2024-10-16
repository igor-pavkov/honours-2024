static int jpeg2000_decode_frame(AVCodecContext *avctx, void *data,
                                 int *got_frame, AVPacket *avpkt)
{
    Jpeg2000DecoderContext *s = avctx->priv_data;
    ThreadFrame frame = { .f = data };
    AVFrame *picture = data;
    int tileno, ret;

    s->avctx     = avctx;
    bytestream2_init(&s->g, avpkt->data, avpkt->size);
    s->curtileno = -1;
    memset(s->cdef, -1, sizeof(s->cdef));

    if (bytestream2_get_bytes_left(&s->g) < 2) {
        ret = AVERROR_INVALIDDATA;
        goto end;
    }

    // check if the image is in jp2 format
    if (bytestream2_get_bytes_left(&s->g) >= 12 &&
       (bytestream2_get_be32u(&s->g) == 12) &&
       (bytestream2_get_be32u(&s->g) == JP2_SIG_TYPE) &&
       (bytestream2_get_be32u(&s->g) == JP2_SIG_VALUE)) {
        if (!jp2_find_codestream(s)) {
            av_log(avctx, AV_LOG_ERROR,
                   "Could not find Jpeg2000 codestream atom.\n");
            ret = AVERROR_INVALIDDATA;
            goto end;
        }
    } else {
        bytestream2_seek(&s->g, 0, SEEK_SET);
    }

    while (bytestream2_get_bytes_left(&s->g) >= 3 && bytestream2_peek_be16(&s->g) != JPEG2000_SOC)
        bytestream2_skip(&s->g, 1);

    if (bytestream2_get_be16u(&s->g) != JPEG2000_SOC) {
        av_log(avctx, AV_LOG_ERROR, "SOC marker not present\n");
        ret = AVERROR_INVALIDDATA;
        goto end;
    }
    if (ret = jpeg2000_read_main_headers(s))
        goto end;

    /* get picture buffer */
    if ((ret = ff_thread_get_buffer(avctx, &frame, 0)) < 0)
        goto end;
    picture->pict_type = AV_PICTURE_TYPE_I;
    picture->key_frame = 1;

    if (ret = jpeg2000_read_bitstream_packets(s))
        goto end;

    for (tileno = 0; tileno < s->numXtiles * s->numYtiles; tileno++)
        if (ret = jpeg2000_decode_tile(s, s->tile + tileno, picture))
            goto end;

    jpeg2000_dec_cleanup(s);

    *got_frame = 1;

    if (s->avctx->pix_fmt == AV_PIX_FMT_PAL8)
        memcpy(picture->data[1], s->palette, 256 * sizeof(uint32_t));

    return bytestream2_tell(&s->g);

end:
    jpeg2000_dec_cleanup(s);
    return ret;
}