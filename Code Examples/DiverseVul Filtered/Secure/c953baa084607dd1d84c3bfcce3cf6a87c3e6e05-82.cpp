static int mov_read_dvcc_dvvc(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    uint8_t buf[ISOM_DVCC_DVVC_SIZE];
    int ret;
    int64_t read_size = atom.size;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    // At most 24 bytes
    read_size = FFMIN(read_size, ISOM_DVCC_DVVC_SIZE);

    if ((ret = ffio_read_size(pb, buf, read_size)) < 0)
        return ret;

    return ff_isom_parse_dvcc_dvvc(c->fc, st, buf, read_size);
}