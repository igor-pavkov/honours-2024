static int mov_read_colr(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    uint8_t *icc_profile;
    char color_parameter_type[5] = { 0 };
    uint16_t color_primaries, color_trc, color_matrix;
    int ret;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams - 1];

    ret = ffio_read_size(pb, color_parameter_type, 4);
    if (ret < 0)
        return ret;
    if (strncmp(color_parameter_type, "nclx", 4) &&
        strncmp(color_parameter_type, "nclc", 4) &&
        strncmp(color_parameter_type, "prof", 4)) {
        av_log(c->fc, AV_LOG_WARNING, "unsupported color_parameter_type %s\n",
               color_parameter_type);
        return 0;
    }

    if (!strncmp(color_parameter_type, "prof", 4)) {
        icc_profile = av_stream_new_side_data(st, AV_PKT_DATA_ICC_PROFILE, atom.size - 4);
        if (!icc_profile)
            return AVERROR(ENOMEM);
        ret = ffio_read_size(pb, icc_profile, atom.size - 4);
        if (ret < 0)
            return ret;
    } else {
        color_primaries = avio_rb16(pb);
        color_trc = avio_rb16(pb);
        color_matrix = avio_rb16(pb);

        av_log(c->fc, AV_LOG_TRACE,
               "%s: pri %d trc %d matrix %d",
               color_parameter_type, color_primaries, color_trc, color_matrix);

        if (!strncmp(color_parameter_type, "nclx", 4)) {
            uint8_t color_range = avio_r8(pb) >> 7;
            av_log(c->fc, AV_LOG_TRACE, " full %"PRIu8"", color_range);
            if (color_range)
                st->codecpar->color_range = AVCOL_RANGE_JPEG;
            else
                st->codecpar->color_range = AVCOL_RANGE_MPEG;
        }

        if (!av_color_primaries_name(color_primaries))
            color_primaries = AVCOL_PRI_UNSPECIFIED;
        if (!av_color_transfer_name(color_trc))
            color_trc = AVCOL_TRC_UNSPECIFIED;
        if (!av_color_space_name(color_matrix))
            color_matrix = AVCOL_SPC_UNSPECIFIED;

        st->codecpar->color_primaries = color_primaries;
        st->codecpar->color_trc       = color_trc;
        st->codecpar->color_space     = color_matrix;
        av_log(c->fc, AV_LOG_TRACE, "\n");
    }
    return 0;
}