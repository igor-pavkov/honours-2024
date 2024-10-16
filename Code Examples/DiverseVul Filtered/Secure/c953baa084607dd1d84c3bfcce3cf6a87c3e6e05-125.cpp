static void set_last_stream_little_endian(AVFormatContext *fc)
{
    AVStream *st;

    if (fc->nb_streams < 1)
        return;
    st = fc->streams[fc->nb_streams-1];

    switch (st->codecpar->codec_id) {
    case AV_CODEC_ID_PCM_S16BE:
        st->codecpar->codec_id = AV_CODEC_ID_PCM_S16LE;
        break;
    case AV_CODEC_ID_PCM_S24BE:
        st->codecpar->codec_id = AV_CODEC_ID_PCM_S24LE;
        break;
    case AV_CODEC_ID_PCM_S32BE:
        st->codecpar->codec_id = AV_CODEC_ID_PCM_S32LE;
        break;
    case AV_CODEC_ID_PCM_F32BE:
        st->codecpar->codec_id = AV_CODEC_ID_PCM_F32LE;
        break;
    case AV_CODEC_ID_PCM_F64BE:
        st->codecpar->codec_id = AV_CODEC_ID_PCM_F64LE;
        break;
    default:
        break;
    }
}