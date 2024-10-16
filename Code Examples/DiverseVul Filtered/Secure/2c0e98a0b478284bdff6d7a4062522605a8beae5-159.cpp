static int mov_write_tfdt_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);

    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "tfdt");
    avio_w8(pb, 1); /* version */
    avio_wb24(pb, 0);
    avio_wb64(pb, track->frag_start);
    return update_size(pb, pos);
}