static int decode_wave_header(AVCodecContext *avctx, const uint8_t *header,
                              int header_size)
{
    int len;
    short wave_format;
    GetByteContext gb;

    bytestream2_init(&gb, header, header_size);

    if (bytestream2_get_le32(&gb) != MKTAG('R', 'I', 'F', 'F')) {
        av_log(avctx, AV_LOG_ERROR, "missing RIFF tag\n");
        return AVERROR_INVALIDDATA;
    }

    bytestream2_skip(&gb, 4); /* chunk size */

    if (bytestream2_get_le32(&gb) != MKTAG('W', 'A', 'V', 'E')) {
        av_log(avctx, AV_LOG_ERROR, "missing WAVE tag\n");
        return AVERROR_INVALIDDATA;
    }

    while (bytestream2_get_le32(&gb) != MKTAG('f', 'm', 't', ' ')) {
        len = bytestream2_get_le32(&gb);
        bytestream2_skip(&gb, len);
        if (bytestream2_get_bytes_left(&gb) < 16) {
            av_log(avctx, AV_LOG_ERROR, "no fmt chunk found\n");
            return AVERROR_INVALIDDATA;
        }
    }
    len = bytestream2_get_le32(&gb);

    if (len < 16) {
        av_log(avctx, AV_LOG_ERROR, "fmt chunk was too short\n");
        return AVERROR_INVALIDDATA;
    }

    wave_format = bytestream2_get_le16(&gb);

    switch (wave_format) {
    case WAVE_FORMAT_PCM:
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "unsupported wave format\n");
        return AVERROR(ENOSYS);
    }

    bytestream2_skip(&gb, 2); // skip channels    (already got from shorten header)
    avctx->sample_rate = bytestream2_get_le32(&gb);
    bytestream2_skip(&gb, 4); // skip bit rate    (represents original uncompressed bit rate)
    bytestream2_skip(&gb, 2); // skip block align (not needed)
    avctx->bits_per_coded_sample = bytestream2_get_le16(&gb);

    if (avctx->bits_per_coded_sample != 16) {
        av_log(avctx, AV_LOG_ERROR, "unsupported number of bits per sample\n");
        return AVERROR(ENOSYS);
    }

    len -= 16;
    if (len > 0)
        av_log(avctx, AV_LOG_INFO, "%d header bytes unparsed\n", len);

    return 0;
}