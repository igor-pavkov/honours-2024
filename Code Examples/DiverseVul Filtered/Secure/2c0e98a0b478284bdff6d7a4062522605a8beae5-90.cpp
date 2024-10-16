static int mp4_get_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag = track->par->codec_tag;

    if (!ff_codec_get_tag(ff_mp4_obj_type, track->par->codec_id))
        return 0;

    if      (track->par->codec_id == AV_CODEC_ID_H264)      tag = MKTAG('a','v','c','1');
    else if (track->par->codec_id == AV_CODEC_ID_HEVC)      tag = MKTAG('h','e','v','1');
    else if (track->par->codec_id == AV_CODEC_ID_VP9)       tag = MKTAG('v','p','0','9');
    else if (track->par->codec_id == AV_CODEC_ID_AC3)       tag = MKTAG('a','c','-','3');
    else if (track->par->codec_id == AV_CODEC_ID_EAC3)      tag = MKTAG('e','c','-','3');
    else if (track->par->codec_id == AV_CODEC_ID_DIRAC)     tag = MKTAG('d','r','a','c');
    else if (track->par->codec_id == AV_CODEC_ID_MOV_TEXT)  tag = MKTAG('t','x','3','g');
    else if (track->par->codec_id == AV_CODEC_ID_VC1)       tag = MKTAG('v','c','-','1');
    else if (track->par->codec_type == AVMEDIA_TYPE_VIDEO)  tag = MKTAG('m','p','4','v');
    else if (track->par->codec_type == AVMEDIA_TYPE_AUDIO)  tag = MKTAG('m','p','4','a');
    else if (track->par->codec_id == AV_CODEC_ID_DVD_SUBTITLE)  tag = MKTAG('m','p','4','s');

    return tag;
}