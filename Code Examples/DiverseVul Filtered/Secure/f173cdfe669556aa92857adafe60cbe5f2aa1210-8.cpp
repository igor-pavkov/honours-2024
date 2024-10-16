static int mxf_set_audio_pts(MXFContext *mxf, AVCodecContext *codec,
                             AVPacket *pkt)
{
    MXFTrack *track = mxf->fc->streams[pkt->stream_index]->priv_data;
    int64_t bits_per_sample = codec->bits_per_coded_sample;

    if (!bits_per_sample)
        bits_per_sample = av_get_bits_per_sample(codec->codec_id);

    pkt->pts = track->sample_count;

    if (   codec->channels <= 0
        || bits_per_sample <= 0
        || codec->channels * (int64_t)bits_per_sample < 8)
        return AVERROR(EINVAL);
    track->sample_count += pkt->size / (codec->channels * (int64_t)bits_per_sample / 8);
    return 0;
}