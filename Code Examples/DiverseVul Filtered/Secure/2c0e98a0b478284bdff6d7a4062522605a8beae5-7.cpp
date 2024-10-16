static int mov_get_rawvideo_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag = track->par->codec_tag;
    int i;
    enum AVPixelFormat pix_fmt;

    for (i = 0; i < FF_ARRAY_ELEMS(mov_pix_fmt_tags); i++) {
        if (track->par->format == mov_pix_fmt_tags[i].pix_fmt) {
            tag = mov_pix_fmt_tags[i].tag;
            track->par->bits_per_coded_sample = mov_pix_fmt_tags[i].bps;
            if (track->par->codec_tag == mov_pix_fmt_tags[i].tag)
                break;
        }
    }

    pix_fmt = avpriv_find_pix_fmt(avpriv_pix_fmt_bps_mov,
                                  track->par->bits_per_coded_sample);
    if (tag == MKTAG('r','a','w',' ') &&
        track->par->format != pix_fmt &&
        track->par->format != AV_PIX_FMT_NONE)
        av_log(s, AV_LOG_ERROR, "%s rawvideo cannot be written to mov, output file will be unreadable\n",
               av_get_pix_fmt_name(track->par->format));
    return tag;
}