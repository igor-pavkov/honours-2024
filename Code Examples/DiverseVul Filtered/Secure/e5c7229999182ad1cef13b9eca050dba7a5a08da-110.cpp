AVCodec *avcodec_find_encoder(enum AVCodecID id)
{
    return find_encdec(id, 1);
}