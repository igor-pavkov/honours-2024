static int mov_write_hvcc_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);

    avio_wb32(pb, 0);
    ffio_wfourcc(pb, "hvcC");
    if (track->tag == MKTAG('h','v','c','1'))
        ff_isom_write_hvcc(pb, track->vos_data, track->vos_len, 1);
    else
        ff_isom_write_hvcc(pb, track->vos_data, track->vos_len, 0);
    return update_size(pb, pos);
}