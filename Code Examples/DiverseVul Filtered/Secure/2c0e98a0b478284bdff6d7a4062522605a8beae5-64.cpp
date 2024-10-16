static int mov_write_wfex_tag(AVFormatContext *s, AVIOContext *pb, MOVTrack *track)
{
    int ret;
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0);
    ffio_wfourcc(pb, "wfex");
    if ((ret = ff_put_wav_header(s, pb, track->st->codecpar, FF_PUT_WAV_HEADER_FORCE_WAVEFORMATEX)) < 0)
        return ret;
    return update_size(pb, pos);
}