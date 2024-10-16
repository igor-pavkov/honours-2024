static int mov_read_mdcv(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVStreamContext *sc;
    const int mapping[3] = {1, 2, 0};
    const int chroma_den = 50000;
    const int luma_den = 10000;
    int i;

    if (c->fc->nb_streams < 1)
        return AVERROR_INVALIDDATA;

    sc = c->fc->streams[c->fc->nb_streams - 1]->priv_data;

    if (atom.size < 24 || sc->mastering) {
        av_log(c->fc, AV_LOG_ERROR, "Invalid Mastering Display Color Volume box\n");
        return AVERROR_INVALIDDATA;
    }

    sc->mastering = av_mastering_display_metadata_alloc();
    if (!sc->mastering)
        return AVERROR(ENOMEM);

    for (i = 0; i < 3; i++) {
        const int j = mapping[i];
        sc->mastering->display_primaries[j][0] = av_make_q(avio_rb16(pb), chroma_den);
        sc->mastering->display_primaries[j][1] = av_make_q(avio_rb16(pb), chroma_den);
    }
    sc->mastering->white_point[0] = av_make_q(avio_rb16(pb), chroma_den);
    sc->mastering->white_point[1] = av_make_q(avio_rb16(pb), chroma_den);

    sc->mastering->max_luminance = av_make_q(avio_rb32(pb), luma_den);
    sc->mastering->min_luminance = av_make_q(avio_rb32(pb), luma_den);

    sc->mastering->has_luminance = 1;
    sc->mastering->has_primaries = 1;

    return 0;
}