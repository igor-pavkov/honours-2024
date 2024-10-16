static int mov_get_dv_codec_tag(AVFormatContext *s, MOVTrack *track)
{
    int tag;

    if (track->par->width == 720) { /* SD */
        if (track->par->height == 480) { /* NTSC */
            if  (track->par->format == AV_PIX_FMT_YUV422P) tag = MKTAG('d','v','5','n');
            else                                            tag = MKTAG('d','v','c',' ');
       }else if (track->par->format == AV_PIX_FMT_YUV422P) tag = MKTAG('d','v','5','p');
        else if (track->par->format == AV_PIX_FMT_YUV420P) tag = MKTAG('d','v','c','p');
        else                                                tag = MKTAG('d','v','p','p');
    } else if (track->par->height == 720) { /* HD 720 line */
        if  (track->st->time_base.den == 50)                tag = MKTAG('d','v','h','q');
        else                                                tag = MKTAG('d','v','h','p');
    } else if (track->par->height == 1080) { /* HD 1080 line */
        if  (track->st->time_base.den == 25)                tag = MKTAG('d','v','h','5');
        else                                                tag = MKTAG('d','v','h','6');
    } else {
        av_log(s, AV_LOG_ERROR, "unsupported height for dv codec\n");
        return 0;
    }

    return tag;
}