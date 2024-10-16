static int mov_write_glbl_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, track->vos_len + 8);
    ffio_wfourcc(pb, "glbl");
    avio_write(pb, track->vos_data, track->vos_len);
    return 8 + track->vos_len;
}