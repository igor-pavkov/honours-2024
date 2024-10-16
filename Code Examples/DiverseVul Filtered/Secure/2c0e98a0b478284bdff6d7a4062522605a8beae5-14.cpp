static int mov_get_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag = track->par->codec_tag;

    if (!tag || (s->strict_std_compliance >= FF_COMPLIANCE_NORMAL &&
                 (track->par->codec_id == AV_CODEC_ID_DVVIDEO ||
                  track->par->codec_id == AV_CODEC_ID_RAWVIDEO ||
                  track->par->codec_id == AV_CODEC_ID_H263 ||
                  track->par->codec_id == AV_CODEC_ID_H264 ||
                  track->par->codec_id == AV_CODEC_ID_DNXHD ||
                  track->par->codec_id == AV_CODEC_ID_MPEG2VIDEO ||
                  av_get_bits_per_sample(track->par->codec_id)))) { // pcm audio
        if (track->par->codec_id == AV_CODEC_ID_DVVIDEO)
            tag = mov_get_dv_codec_tag(s, track);
        else if (track->par->codec_id == AV_CODEC_ID_RAWVIDEO)
            tag = mov_get_rawvideo_codec_tag(s, track);
        else if (track->par->codec_id == AV_CODEC_ID_MPEG2VIDEO)
            tag = mov_get_mpeg2_xdcam_codec_tag(s, track);
        else if (track->par->codec_id == AV_CODEC_ID_H264)
            tag = mov_get_h264_codec_tag(s, track);
        else if (track->par->codec_id == AV_CODEC_ID_DNXHD)
            tag = mov_get_dnxhd_codec_tag(s, track);
        else if (track->par->codec_type == AVMEDIA_TYPE_VIDEO) {
            tag = ff_codec_get_tag(ff_codec_movvideo_tags, track->par->codec_id);
            if (!tag) { // if no mac fcc found, try with Microsoft tags
                tag = ff_codec_get_tag(ff_codec_bmp_tags, track->par->codec_id);
                if (tag)
                    av_log(s, AV_LOG_WARNING, "Using MS style video codec tag, "
                           "the file may be unplayable!\n");
            }
        } else if (track->par->codec_type == AVMEDIA_TYPE_AUDIO) {
            tag = ff_codec_get_tag(ff_codec_movaudio_tags, track->par->codec_id);
            if (!tag) { // if no mac fcc found, try with Microsoft tags
                int ms_tag = ff_codec_get_tag(ff_codec_wav_tags, track->par->codec_id);
                if (ms_tag) {
                    tag = MKTAG('m', 's', ((ms_tag >> 8) & 0xff), (ms_tag & 0xff));
                    av_log(s, AV_LOG_WARNING, "Using MS style audio codec tag, "
                           "the file may be unplayable!\n");
                }
            }
        } else if (track->par->codec_type == AVMEDIA_TYPE_SUBTITLE)
            tag = ff_codec_get_tag(ff_codec_movsubtitle_tags, track->par->codec_id);
    }

    return tag;
}