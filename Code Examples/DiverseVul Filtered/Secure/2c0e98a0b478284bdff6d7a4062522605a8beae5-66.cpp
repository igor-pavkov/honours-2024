static int mov_find_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag;

    if (track->mode == MODE_MP4 || track->mode == MODE_PSP)
        tag = mp4_get_codec_tag(s, track);
    else if (track->mode == MODE_ISM) {
        tag = mp4_get_codec_tag(s, track);
        if (!tag && track->par->codec_id == AV_CODEC_ID_WMAPRO)
            tag = MKTAG('w', 'm', 'a', ' ');
    } else if (track->mode == MODE_IPOD)
        tag = ipod_get_codec_tag(s, track);
    else if (track->mode & MODE_3GP)
        tag = ff_codec_get_tag(codec_3gp_tags, track->par->codec_id);
    else if (track->mode == MODE_F4V)
        tag = ff_codec_get_tag(codec_f4v_tags, track->par->codec_id);
    else
        tag = mov_get_codec_tag(s, track);

    return tag;
}