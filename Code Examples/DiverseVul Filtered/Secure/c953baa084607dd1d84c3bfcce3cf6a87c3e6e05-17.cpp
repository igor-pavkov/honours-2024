static int mov_read_enda(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    int little_endian = avio_rb16(pb) & 0xFF;
    av_log(c->fc, AV_LOG_TRACE, "enda %d\n", little_endian);
    if (little_endian == 1)
        set_last_stream_little_endian(c->fc);
    return 0;
}