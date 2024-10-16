static int mov_read_pasp(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    const int num = avio_rb32(pb);
    const int den = avio_rb32(pb);
    AVStream *st;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    if (den != 0) {
        av_reduce(&st->sample_aspect_ratio.num, &st->sample_aspect_ratio.den,
                  num, den, 32767);
    }
    return 0;
}