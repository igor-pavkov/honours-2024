static int mov_write_stsd_tag(AVFormatContext *s, AVIOContext *pb, MOVMuxContext *mov, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "stsd");
    avio_wb32(pb, 0); /* version & flags */
    avio_wb32(pb, 1); /* entry count */
    if (track->par->codec_type == AVMEDIA_TYPE_VIDEO)
        mov_write_video_tag(pb, mov, track);
    else if (track->par->codec_type == AVMEDIA_TYPE_AUDIO)
        mov_write_audio_tag(s, pb, mov, track);
    else if (track->par->codec_type == AVMEDIA_TYPE_SUBTITLE)
        mov_write_subtitle_tag(pb, track);
    else if (track->par->codec_tag == MKTAG('r','t','p',' '))
        mov_write_rtp_tag(pb, track);
    else if (track->par->codec_tag == MKTAG('t','m','c','d'))
        mov_write_tmcd_tag(pb, track);
    return update_size(pb, pos);
}