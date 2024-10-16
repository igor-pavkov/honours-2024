static int rtp_hinting_needed(const AVStream *st)
{
    /* Add hint tracks for each real audio and video stream */
    if (is_cover_image(st))
        return 0;
    return st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ||
           st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO;
}