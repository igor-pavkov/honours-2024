static int mov_write_ms_tag(AVFormatContext *s, AVIOContext *pb, MOVTrack *track)
{
    int ret;
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0);
    avio_wl32(pb, track->tag); // store it byteswapped
    track->par->codec_tag = av_bswap16(track->tag >> 16);
    if ((ret = ff_put_wav_header(s, pb, track->par, 0)) < 0)
        return ret;
    return update_size(pb, pos);
}