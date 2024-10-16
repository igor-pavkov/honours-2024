static int mov_write_track_udta_tag(AVIOContext *pb, MOVMuxContext *mov,
                                    AVStream *st)
{
    AVIOContext *pb_buf;
    int ret, size;
    uint8_t *buf;

    if (!st)
        return 0;

    ret = avio_open_dyn_buf(&pb_buf);
    if (ret < 0)
        return ret;

    if (mov->mode & MODE_MP4)
        mov_write_track_metadata(pb_buf, st, "name", "title");

    if ((size = avio_close_dyn_buf(pb_buf, &buf)) > 0) {
        avio_wb32(pb, size + 8);
        ffio_wfourcc(pb, "udta");
        avio_write(pb, buf, size);
    }
    av_free(buf);

    return 0;
}