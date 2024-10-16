static int fixup_vorbis_headers(AVFormatContext *as,
                                struct oggvorbis_private *priv,
                                uint8_t **buf)
{
    int i, offset, len, err;
    int buf_len;
    unsigned char *ptr;

    len = priv->len[0] + priv->len[1] + priv->len[2];
    buf_len = len + len / 255 + 64;
    ptr = *buf = av_realloc(NULL, buf_len);
    if (!ptr)
        return AVERROR(ENOMEM);
    memset(*buf, '\0', buf_len);

    ptr[0]  = 2;
    offset  = 1;
    offset += av_xiphlacing(&ptr[offset], priv->len[0]);
    offset += av_xiphlacing(&ptr[offset], priv->len[1]);
    for (i = 0; i < 3; i++) {
        memcpy(&ptr[offset], priv->packet[i], priv->len[i]);
        offset += priv->len[i];
        av_freep(&priv->packet[i]);
    }
    if ((err = av_reallocp(buf, offset + FF_INPUT_BUFFER_PADDING_SIZE)) < 0)
        return err;
    return offset;
}