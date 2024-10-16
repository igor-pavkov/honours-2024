static int mov_auto_flush_fragment(AVFormatContext *s, int force)
{
    MOVMuxContext *mov = s->priv_data;
    int had_moov = mov->moov_written;
    int ret = mov_flush_fragment(s, force);
    if (ret < 0)
        return ret;
    // If using delay_moov, the first flush only wrote the moov,
    // not the actual moof+mdat pair, thus flush once again.
    if (!had_moov && mov->flags & FF_MOV_FLAG_DELAY_MOOV)
        ret = mov_flush_fragment(s, force);
    return ret;
}