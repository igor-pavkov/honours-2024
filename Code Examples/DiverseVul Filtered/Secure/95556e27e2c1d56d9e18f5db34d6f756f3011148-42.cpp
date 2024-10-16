static int mov_find_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag;

    if (is_cover_image(track->st))
        return ff_codec_get_tag(codec_cover_image_tags, track->par->codec_id);

    if (track->mode == MODE_MP4 || track->mode == MODE_PSP)
        tag = track->par->codec_tag;
    else if (track->mode == MODE_ISM)
        tag = track->par->codec_tag;
    else if (track->mode == MODE_IPOD) {
        if (!av_match_ext(s->url, "m4a") &&
            !av_match_ext(s->url, "m4v") &&
            !av_match_ext(s->url, "m4b"))
            av_log(s, AV_LOG_WARNING, "Warning, extension is not .m4a nor .m4v "
                   "Quicktime/Ipod might not play the file\n");
        tag = track->par->codec_tag;
    } else if (track->mode & MODE_3GP)
        tag = track->par->codec_tag;
    else if (track->mode == MODE_F4V)
        tag = track->par->codec_tag;
    else
        tag = mov_get_codec_tag(s, track);

    return tag;
}