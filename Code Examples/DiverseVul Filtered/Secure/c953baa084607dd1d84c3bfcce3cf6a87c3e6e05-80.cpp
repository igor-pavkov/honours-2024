static int mov_read_dmlp(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    unsigned format_info;
    int channel_assignment, channel_assignment1, channel_assignment2;
    int ratebits;
    uint64_t chmask;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    if (atom.size < 10)
        return AVERROR_INVALIDDATA;

    format_info = avio_rb32(pb);

    ratebits            = (format_info >> 28) & 0xF;
    channel_assignment1 = (format_info >> 15) & 0x1F;
    channel_assignment2 = format_info & 0x1FFF;
    if (channel_assignment2)
        channel_assignment = channel_assignment2;
    else
        channel_assignment = channel_assignment1;

    st->codecpar->frame_size = 40 << (ratebits & 0x7);
    st->codecpar->sample_rate = mlp_samplerate(ratebits);

    av_channel_layout_uninit(&st->codecpar->ch_layout);
    chmask = truehd_layout(channel_assignment);
    av_channel_layout_from_mask(&st->codecpar->ch_layout, chmask);

    return 0;
}