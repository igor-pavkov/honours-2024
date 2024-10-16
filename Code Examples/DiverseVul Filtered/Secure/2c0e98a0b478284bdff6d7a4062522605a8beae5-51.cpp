static int mov_write_tfra_tag(AVIOContext *pb, MOVTrack *track)
{
    int64_t pos = avio_tell(pb);
    int i;

    avio_wb32(pb, 0); /* size placeholder */
    ffio_wfourcc(pb, "tfra");
    avio_w8(pb, 1); /* version */
    avio_wb24(pb, 0);

    avio_wb32(pb, track->track_id);
    avio_wb32(pb, 0); /* length of traf/trun/sample num */
    avio_wb32(pb, track->nb_frag_info);
    for (i = 0; i < track->nb_frag_info; i++) {
        avio_wb64(pb, track->frag_info[i].time);
        avio_wb64(pb, track->frag_info[i].offset + track->data_offset);
        avio_w8(pb, 1); /* traf number */
        avio_w8(pb, 1); /* trun number */
        avio_w8(pb, 1); /* sample number */
    }

    return update_size(pb, pos);
}