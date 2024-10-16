static int mov_write_dvc1_tag(AVIOContext *pb, MOVTrack *track)
{
    uint8_t buf[7] = { 0 };
    int ret;

    if ((ret = mov_write_dvc1_structs(track, buf)) < 0)
        return ret;

    avio_wb32(pb, track->vos_len + 8 + sizeof(buf));
    ffio_wfourcc(pb, "dvc1");
    avio_write(pb, buf, sizeof(buf));
    avio_write(pb, track->vos_data, track->vos_len);

    return 0;
}