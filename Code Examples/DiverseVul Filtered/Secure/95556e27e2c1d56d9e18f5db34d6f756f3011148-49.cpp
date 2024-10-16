static int mov_write_st3d_tag(AVIOContext *pb, AVStereo3D *stereo_3d)
{
    int8_t stereo_mode;

    if (stereo_3d->flags != 0) {
        av_log(pb, AV_LOG_WARNING, "Unsupported stereo_3d flags %x. st3d not written.\n", stereo_3d->flags);
        return 0;
    }

    switch (stereo_3d->type) {
    case AV_STEREO3D_2D:
        stereo_mode = 0;
        break;
    case AV_STEREO3D_TOPBOTTOM:
        stereo_mode = 1;
        break;
    case AV_STEREO3D_SIDEBYSIDE:
        stereo_mode = 2;
        break;
    default:
        av_log(pb, AV_LOG_WARNING, "Unsupported stereo_3d type %s. st3d not written.\n", av_stereo3d_type_name(stereo_3d->type));
        return 0;
    }
    avio_wb32(pb, 13); /* size */
    ffio_wfourcc(pb, "st3d");
    avio_wb32(pb, 0); /* version = 0 & flags = 0 */
    avio_w8(pb, stereo_mode);
    return 13;
}