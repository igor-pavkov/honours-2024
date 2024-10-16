static int ipod_get_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag = track->par->codec_tag;

    // keep original tag for subs, ipod supports both formats
    if (!(track->par->codec_type == AVMEDIA_TYPE_SUBTITLE &&
          (tag == MKTAG('t', 'x', '3', 'g') ||
           tag == MKTAG('t', 'e', 'x', 't'))))
        tag = ff_codec_get_tag(codec_ipod_tags, track->par->codec_id);

    if (!av_match_ext(s->filename, "m4a") &&
        !av_match_ext(s->filename, "m4b") &&
        !av_match_ext(s->filename, "m4v"))
        av_log(s, AV_LOG_WARNING, "Warning, extension is not .m4a, .m4v nor  .m4b "
               "Quicktime/Ipod might not play the file\n");

    return tag;
}