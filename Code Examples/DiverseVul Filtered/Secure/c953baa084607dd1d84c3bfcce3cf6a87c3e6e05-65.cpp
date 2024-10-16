static int mov_read_strf(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    int ret;

    if (c->fc->nb_streams < 1)
        return 0;
    if (atom.size <= 40)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    if ((uint64_t)atom.size > (1<<30))
        return AVERROR_INVALIDDATA;

    avio_skip(pb, 40);
    ret = ff_get_extradata(c->fc, st->codecpar, pb, atom.size - 40);
    if (ret < 0)
        return ret;

    return 0;
}