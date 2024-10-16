static int flv_write_trailer(AVFormatContext *s)
{
    int64_t file_size;

    AVIOContext *pb = s->pb;
    FLVContext *flv = s->priv_data;
    int i;

    /* Add EOS tag */
    for (i = 0; i < s->nb_streams; i++) {
        AVCodecContext *enc = s->streams[i]->codec;
        FLVStreamContext *sc = s->streams[i]->priv_data;
        if (enc->codec_type == AVMEDIA_TYPE_VIDEO &&
                (enc->codec_id == AV_CODEC_ID_H264 || enc->codec_id == AV_CODEC_ID_MPEG4))
            put_avc_eos_tag(pb, sc->last_ts);
    }

    file_size = avio_tell(pb);

    /* update information */
    if (avio_seek(pb, flv->duration_offset, SEEK_SET) < 0)
        av_log(s, AV_LOG_WARNING, "Failed to update header with correct duration.\n");
    else
        put_amf_double(pb, flv->duration / (double)1000);
    if (avio_seek(pb, flv->filesize_offset, SEEK_SET) < 0)
        av_log(s, AV_LOG_WARNING, "Failed to update header with correct filesize.\n");
    else
        put_amf_double(pb, file_size);

    avio_seek(pb, file_size, SEEK_SET);
    return 0;
}