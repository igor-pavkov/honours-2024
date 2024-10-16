static int mov_read_smdm(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVStreamContext *sc;
    int i, version;

    if (c->fc->nb_streams < 1)
        return AVERROR_INVALIDDATA;

    sc = c->fc->streams[c->fc->nb_streams - 1]->priv_data;

    if (atom.size < 5) {
        av_log(c->fc, AV_LOG_ERROR, "Empty Mastering Display Metadata box\n");
        return AVERROR_INVALIDDATA;
    }

    version = avio_r8(pb);
    if (version) {
        av_log(c->fc, AV_LOG_WARNING, "Unsupported Mastering Display Metadata box version %d\n", version);
        return 0;
    }
    if (sc->mastering)
        return AVERROR_INVALIDDATA;

    avio_skip(pb, 3); /* flags */

    sc->mastering = av_mastering_display_metadata_alloc();
    if (!sc->mastering)
        return AVERROR(ENOMEM);

    for (i = 0; i < 3; i++) {
        sc->mastering->display_primaries[i][0] = av_make_q(avio_rb16(pb), 1 << 16);
        sc->mastering->display_primaries[i][1] = av_make_q(avio_rb16(pb), 1 << 16);
    }
    sc->mastering->white_point[0] = av_make_q(avio_rb16(pb), 1 << 16);
    sc->mastering->white_point[1] = av_make_q(avio_rb16(pb), 1 << 16);

    sc->mastering->max_luminance = av_make_q(avio_rb32(pb), 1 << 8);
    sc->mastering->min_luminance = av_make_q(avio_rb32(pb), 1 << 14);

    sc->mastering->has_primaries = 1;
    sc->mastering->has_luminance = 1;

    return 0;
}