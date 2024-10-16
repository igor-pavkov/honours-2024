static int mov_read_glbl(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    int ret;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    if ((uint64_t)atom.size > (1<<30))
        return AVERROR_INVALIDDATA;

    if (atom.size >= 10) {
        // Broken files created by legacy versions of libavformat will
        // wrap a whole fiel atom inside of a glbl atom.
        unsigned size = avio_rb32(pb);
        unsigned type = avio_rl32(pb);
        if (avio_feof(pb))
            return AVERROR_INVALIDDATA;
        avio_seek(pb, -8, SEEK_CUR);
        if (type == MKTAG('f','i','e','l') && size == atom.size)
            return mov_read_default(c, pb, atom);
    }
    if (st->codecpar->extradata_size > 1 && st->codecpar->extradata) {
        av_log(c->fc, AV_LOG_WARNING, "ignoring multiple glbl\n");
        return 0;
    }
    ret = ff_get_extradata(c->fc, st->codecpar, pb, atom.size);
    if (ret < 0)
        return ret;
    if (atom.type == MKTAG('h','v','c','C') && st->codecpar->codec_tag == MKTAG('d','v','h','1'))
        /* HEVC-based Dolby Vision derived from hvc1.
           Happens to match with an identifier
           previously utilized for DV. Thus, if we have
           the hvcC extradata box available as specified,
           set codec to HEVC */
        st->codecpar->codec_id = AV_CODEC_ID_HEVC;

    return 0;
}