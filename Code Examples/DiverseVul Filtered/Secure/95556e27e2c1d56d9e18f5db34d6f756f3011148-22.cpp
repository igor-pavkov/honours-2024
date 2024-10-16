static int mov_write_covr(AVIOContext *pb, AVFormatContext *s)
{
    MOVMuxContext *mov = s->priv_data;
    int64_t pos = 0;
    int i;

    for (i = 0; i < s->nb_streams; i++) {
        MOVTrack *trk = &mov->tracks[i];

        if (!is_cover_image(trk->st) || trk->cover_image.size <= 0)
            continue;

        if (!pos) {
            pos = avio_tell(pb);
            avio_wb32(pb, 0);
            ffio_wfourcc(pb, "covr");
        }
        avio_wb32(pb, 16 + trk->cover_image.size);
        ffio_wfourcc(pb, "data");
        avio_wb32(pb, trk->tag);
        avio_wb32(pb , 0);
        avio_write(pb, trk->cover_image.data, trk->cover_image.size);
    }

    return pos ? update_size(pb, pos) : 0;
}