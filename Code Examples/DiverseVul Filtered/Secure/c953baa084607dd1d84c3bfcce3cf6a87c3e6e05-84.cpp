static int mov_read_ddts(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
#define DDTS_SIZE 20
    uint8_t buf[DDTS_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    AVStream *st = NULL;
    uint32_t frame_duration_code = 0;
    uint32_t channel_layout_code = 0;
    GetBitContext gb;
    int ret;

    if ((ret = ffio_read_size(pb, buf, DDTS_SIZE)) < 0)
        return ret;

    init_get_bits(&gb, buf, 8 * DDTS_SIZE);

    if (c->fc->nb_streams < 1) {
        return 0;
    }
    st = c->fc->streams[c->fc->nb_streams-1];

    st->codecpar->sample_rate = get_bits_long(&gb, 32);
    if (st->codecpar->sample_rate <= 0) {
        av_log(c->fc, AV_LOG_ERROR, "Invalid sample rate %d\n", st->codecpar->sample_rate);
        return AVERROR_INVALIDDATA;
    }
    skip_bits_long(&gb, 32); /* max bitrate */
    st->codecpar->bit_rate = get_bits_long(&gb, 32);
    st->codecpar->bits_per_coded_sample = get_bits(&gb, 8);
    frame_duration_code = get_bits(&gb, 2);
    skip_bits(&gb, 30); /* various fields */
    channel_layout_code = get_bits(&gb, 16);

    st->codecpar->frame_size =
            (frame_duration_code == 0) ? 512 :
            (frame_duration_code == 1) ? 1024 :
            (frame_duration_code == 2) ? 2048 :
            (frame_duration_code == 3) ? 4096 : 0;

    if (channel_layout_code > 0xff) {
        av_log(c->fc, AV_LOG_WARNING, "Unsupported DTS audio channel layout\n");
    }
    av_channel_layout_uninit(&st->codecpar->ch_layout);
    av_channel_layout_from_mask(&st->codecpar->ch_layout,
            ((channel_layout_code & 0x1) ? AV_CH_FRONT_CENTER : 0) |
            ((channel_layout_code & 0x2) ? AV_CH_FRONT_LEFT : 0) |
            ((channel_layout_code & 0x2) ? AV_CH_FRONT_RIGHT : 0) |
            ((channel_layout_code & 0x4) ? AV_CH_SIDE_LEFT : 0) |
            ((channel_layout_code & 0x4) ? AV_CH_SIDE_RIGHT : 0) |
            ((channel_layout_code & 0x8) ? AV_CH_LOW_FREQUENCY : 0));

    return 0;
}