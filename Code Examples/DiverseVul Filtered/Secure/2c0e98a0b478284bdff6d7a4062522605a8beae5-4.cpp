static int mov_write_mvex_tag(AVIOContext *pb, MOVMuxContext *mov)
{
    int64_t pos = avio_tell(pb);
    int i;
    avio_wb32(pb, 0x0); /* size */
    ffio_wfourcc(pb, "mvex");
    for (i = 0; i < mov->nb_streams; i++)
        mov_write_trex_tag(pb, &mov->tracks[i]);
    return update_size(pb, pos);
}