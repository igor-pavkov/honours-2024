static int mov_write_tref_tag(AVIOContext *pb, MOVTrack *track)
{
    avio_wb32(pb, 20);   // size
    ffio_wfourcc(pb, "tref");
    avio_wb32(pb, 12);   // size (subatom)
    avio_wl32(pb, track->tref_tag);
    avio_wb32(pb, track->tref_id);
    return 20;
}