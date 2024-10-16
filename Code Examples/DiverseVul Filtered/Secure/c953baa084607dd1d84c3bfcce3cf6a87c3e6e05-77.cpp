static int mov_read_dac3(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    enum AVAudioServiceType *ast;
    int ac3info, acmod, lfeon, bsmod;
    uint64_t mask;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    ast = (enum AVAudioServiceType*)av_stream_new_side_data(st, AV_PKT_DATA_AUDIO_SERVICE_TYPE,
                                                            sizeof(*ast));
    if (!ast)
        return AVERROR(ENOMEM);

    ac3info = avio_rb24(pb);
    bsmod = (ac3info >> 14) & 0x7;
    acmod = (ac3info >> 11) & 0x7;
    lfeon = (ac3info >> 10) & 0x1;

    mask = ff_ac3_channel_layout_tab[acmod];
    if (lfeon)
        mask |= AV_CH_LOW_FREQUENCY;
    av_channel_layout_uninit(&st->codecpar->ch_layout);
    av_channel_layout_from_mask(&st->codecpar->ch_layout, mask);

    *ast = bsmod;
    if (st->codecpar->ch_layout.nb_channels > 1 && bsmod == 0x7)
        *ast = AV_AUDIO_SERVICE_TYPE_KARAOKE;

    return 0;
}