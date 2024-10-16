static int mov_write_sv3d_tag(AVFormatContext *s, AVIOContext *pb, AVSphericalMapping *spherical_mapping)
{
    int64_t sv3d_pos, svhd_pos, proj_pos;
    const char* metadata_source = s->flags & AVFMT_FLAG_BITEXACT ? "Lavf" : LIBAVFORMAT_IDENT;

    if (spherical_mapping->projection != AV_SPHERICAL_EQUIRECTANGULAR &&
        spherical_mapping->projection != AV_SPHERICAL_EQUIRECTANGULAR_TILE &&
        spherical_mapping->projection != AV_SPHERICAL_CUBEMAP) {
        av_log(pb, AV_LOG_WARNING, "Unsupported projection %d. sv3d not written.\n", spherical_mapping->projection);
        return 0;
    }

    sv3d_pos = avio_tell(pb);
    avio_wb32(pb, 0);  /* size */
    ffio_wfourcc(pb, "sv3d");

    svhd_pos = avio_tell(pb);
    avio_wb32(pb, 0);  /* size */
    ffio_wfourcc(pb, "svhd");
    avio_wb32(pb, 0); /* version = 0 & flags = 0 */
    avio_put_str(pb, metadata_source);
    update_size(pb, svhd_pos);

    proj_pos = avio_tell(pb);
    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "proj");

    avio_wb32(pb, 24); /* size */
    ffio_wfourcc(pb, "prhd");
    avio_wb32(pb, 0); /* version = 0 & flags = 0 */
    avio_wb32(pb, spherical_mapping->yaw);
    avio_wb32(pb, spherical_mapping->pitch);
    avio_wb32(pb, spherical_mapping->roll);

    switch (spherical_mapping->projection) {
    case AV_SPHERICAL_EQUIRECTANGULAR:
    case AV_SPHERICAL_EQUIRECTANGULAR_TILE:
        avio_wb32(pb, 28);    /* size */
        ffio_wfourcc(pb, "equi");
        avio_wb32(pb, 0); /* version = 0 & flags = 0 */
        avio_wb32(pb, spherical_mapping->bound_top);
        avio_wb32(pb, spherical_mapping->bound_bottom);
        avio_wb32(pb, spherical_mapping->bound_left);
        avio_wb32(pb, spherical_mapping->bound_right);
        break;
    case AV_SPHERICAL_CUBEMAP:
        avio_wb32(pb, 20);    /* size */
        ffio_wfourcc(pb, "cbmp");
        avio_wb32(pb, 0); /* version = 0 & flags = 0 */
        avio_wb32(pb, 0); /* layout */
        avio_wb32(pb, spherical_mapping->padding); /* padding */
        break;
    }
    update_size(pb, proj_pos);

    return update_size(pb, sv3d_pos);
}