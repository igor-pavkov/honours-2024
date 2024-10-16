static int mov_write_stco_tag(AVIOContext *pb, MOVTrack *track)
{
    int i;
    int mode64 = co64_required(track); // use 32 bit size variant if possible
    int64_t pos = avio_tell(pb);
    avio_wb32(pb, 0); /* size */
    if (mode64)
        ffio_wfourcc(pb, "co64");
    else
        ffio_wfourcc(pb, "stco");
    avio_wb32(pb, 0); /* version & flags */
    avio_wb32(pb, track->chunkCount); /* entry count */
    for (i = 0; i < track->entry; i++) {
        if (!track->cluster[i].chunkNum)
            continue;
        if (mode64 == 1)
            avio_wb64(pb, track->cluster[i].pos + track->data_offset);
        else
            avio_wb32(pb, track->cluster[i].pos + track->data_offset);
    }
    return update_size(pb, pos);
}