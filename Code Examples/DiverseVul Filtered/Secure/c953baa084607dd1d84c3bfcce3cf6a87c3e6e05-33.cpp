static int mov_read_SA3D(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    int i, version, type;
    int ambisonic_order, channel_order, normalization, channel_count;

    if (c->fc->nb_streams < 1)
        return 0;

    st = c->fc->streams[c->fc->nb_streams - 1];

    if (atom.size < 16) {
        av_log(c->fc, AV_LOG_ERROR, "SA3D audio box too small\n");
        return AVERROR_INVALIDDATA;
    }

    version = avio_r8(pb);
    if (version) {
        av_log(c->fc, AV_LOG_WARNING, "Unsupported SA3D box version %d\n", version);
        return 0;
    }

    type = avio_r8(pb);
    if (type) {
        av_log(c->fc, AV_LOG_WARNING,
               "Unsupported ambisonic type %d\n", type);
        return 0;
    }

    ambisonic_order = avio_rb32(pb);

    channel_order = avio_r8(pb);
    if (channel_order) {
        av_log(c->fc, AV_LOG_WARNING,
               "Unsupported channel_order %d\n", channel_order);
        return 0;
    }

    normalization = avio_r8(pb);
    if (normalization) {
        av_log(c->fc, AV_LOG_WARNING,
               "Unsupported normalization %d\n", normalization);
        return 0;
    }

    channel_count = avio_rb32(pb);
    if (ambisonic_order < 0 || channel_count != (ambisonic_order + 1LL) * (ambisonic_order + 1LL)) {
        av_log(c->fc, AV_LOG_ERROR,
               "Invalid number of channels (%d / %d)\n",
               channel_count, ambisonic_order);
        return 0;
    }

    for (i = 0; i < channel_count; i++) {
        if (i != avio_rb32(pb)) {
            av_log(c->fc, AV_LOG_WARNING,
                   "Ambisonic channel reordering is not supported\n");
            return 0;
        }
    }

    av_channel_layout_uninit(&st->codecpar->ch_layout);
    st->codecpar->ch_layout.order = AV_CHANNEL_ORDER_AMBISONIC;
    st->codecpar->ch_layout.nb_channels = channel_count;

    return 0;
}