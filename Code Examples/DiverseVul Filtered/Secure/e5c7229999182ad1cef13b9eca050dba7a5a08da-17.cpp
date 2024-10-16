enum AVMediaType avcodec_get_type(enum AVCodecID codec_id)
{
    AVCodec *c= avcodec_find_decoder(codec_id);
    if(!c)
        c= avcodec_find_encoder(codec_id);
    if(c)
        return c->type;

    if (codec_id <= AV_CODEC_ID_NONE)
        return AVMEDIA_TYPE_UNKNOWN;
    else if (codec_id < AV_CODEC_ID_FIRST_AUDIO)
        return AVMEDIA_TYPE_VIDEO;
    else if (codec_id < AV_CODEC_ID_FIRST_SUBTITLE)
        return AVMEDIA_TYPE_AUDIO;
    else if (codec_id < AV_CODEC_ID_FIRST_UNKNOWN)
        return AVMEDIA_TYPE_SUBTITLE;

    return AVMEDIA_TYPE_UNKNOWN;
}