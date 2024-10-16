static int mov_write_ftyp_tag(AVIOContext *pb, AVFormatContext *s)
{
    MOVMuxContext *mov = s->priv_data;
    int64_t pos = avio_tell(pb);
    int has_h264 = 0, has_video = 0;
    int minor = 0x200;
    int i;

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            has_video = 1;
        if (st->codecpar->codec_id == AV_CODEC_ID_H264)
            has_h264 = 1;
    }

    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "ftyp");

    if (mov->major_brand && strlen(mov->major_brand) >= 4)
        ffio_wfourcc(pb, mov->major_brand);
    else if (mov->mode == MODE_3GP) {
        ffio_wfourcc(pb, has_h264 ? "3gp6"  : "3gp4");
        minor =     has_h264 ?   0x100 :   0x200;
    } else if (mov->mode & MODE_3G2) {
        ffio_wfourcc(pb, has_h264 ? "3g2b"  : "3g2a");
        minor =     has_h264 ? 0x20000 : 0x10000;
    } else if (mov->mode == MODE_PSP)
        ffio_wfourcc(pb, "MSNV");
    else if (mov->mode == MODE_MP4 && mov->flags & FF_MOV_FLAG_DEFAULT_BASE_MOOF)
        ffio_wfourcc(pb, "iso5"); // Required when using default-base-is-moof
    else if (mov->mode == MODE_MP4)
        ffio_wfourcc(pb, "isom");
    else if (mov->mode == MODE_IPOD)
        ffio_wfourcc(pb, has_video ? "M4V ":"M4A ");
    else if (mov->mode == MODE_ISM)
        ffio_wfourcc(pb, "isml");
    else if (mov->mode == MODE_F4V)
        ffio_wfourcc(pb, "f4v ");
    else
        ffio_wfourcc(pb, "qt  ");

    avio_wb32(pb, minor);

    if (mov->mode == MODE_MOV)
        ffio_wfourcc(pb, "qt  ");
    else if (mov->mode == MODE_ISM) {
        ffio_wfourcc(pb, "piff");
    } else if (!(mov->flags & FF_MOV_FLAG_DEFAULT_BASE_MOOF)) {
        ffio_wfourcc(pb, "isom");
        ffio_wfourcc(pb, "iso2");
        if (has_h264)
            ffio_wfourcc(pb, "avc1");
    }

    // We add tfdt atoms when fragmenting, signal this with the iso6 compatible
    // brand. This is compatible with users that don't understand tfdt.
    if (mov->flags & FF_MOV_FLAG_FRAGMENT && mov->mode != MODE_ISM)
        ffio_wfourcc(pb, "iso6");

    if (mov->mode == MODE_3GP)
        ffio_wfourcc(pb, has_h264 ? "3gp6":"3gp4");
    else if (mov->mode & MODE_3G2)
        ffio_wfourcc(pb, has_h264 ? "3g2b":"3g2a");
    else if (mov->mode == MODE_PSP)
        ffio_wfourcc(pb, "MSNV");
    else if (mov->mode == MODE_MP4)
        ffio_wfourcc(pb, "mp41");

    if (mov->flags & FF_MOV_FLAG_DASH && mov->flags & FF_MOV_FLAG_GLOBAL_SIDX)
        ffio_wfourcc(pb, "dash");

    return update_size(pb, pos);
}