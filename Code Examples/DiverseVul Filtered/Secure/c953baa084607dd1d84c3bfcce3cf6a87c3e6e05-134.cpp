static int get_eia608_packet(AVIOContext *pb, AVPacket *pkt, int size)
{
    int new_size, ret;

    if (size <= 8)
        return AVERROR_INVALIDDATA;
    new_size = ((size - 8) / 2) * 3;
    ret = av_new_packet(pkt, new_size);
    if (ret < 0)
        return ret;

    avio_skip(pb, 8);
    for (int j = 0; j < new_size; j += 3) {
        pkt->data[j] = 0xFC;
        pkt->data[j+1] = avio_r8(pb);
        pkt->data[j+2] = avio_r8(pb);
    }

    return 0;
}