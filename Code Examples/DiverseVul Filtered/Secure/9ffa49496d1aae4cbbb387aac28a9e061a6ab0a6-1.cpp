static int adts_init(AVFormatContext *s)
{
    ADTSContext *adts = s->priv_data;
    AVCodecParameters *par = s->streams[0]->codecpar;

    if (par->codec_id != AV_CODEC_ID_AAC) {
        av_log(s, AV_LOG_ERROR, "Only AAC streams can be muxed by the ADTS muxer\n");
        return AVERROR(EINVAL);
    }
    if (par->extradata_size > 0)
        return adts_decode_extradata(s, adts, par->extradata,
                                     par->extradata_size);

    return 0;
}