static int mov_read_SAND(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    int version;

    if (c->fc->nb_streams < 1)
        return 0;

    st = c->fc->streams[c->fc->nb_streams - 1];

    if (atom.size < 5) {
        av_log(c->fc, AV_LOG_ERROR, "Empty SAND audio box\n");
        return AVERROR_INVALIDDATA;
    }

    version = avio_r8(pb);
    if (version) {
        av_log(c->fc, AV_LOG_WARNING, "Unsupported SAND box version %d\n", version);
        return 0;
    }

    st->disposition |= AV_DISPOSITION_NON_DIEGETIC;

    return 0;
}