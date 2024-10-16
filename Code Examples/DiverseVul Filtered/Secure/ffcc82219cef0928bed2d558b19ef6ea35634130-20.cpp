int ff_rtmp_packet_create(RTMPPacket *pkt, int channel_id, RTMPPacketType type,
                          int timestamp, int size)
{
    if (size) {
        pkt->data = av_realloc(NULL, size);
        if (!pkt->data)
            return AVERROR(ENOMEM);
    }
    pkt->size       = size;
    pkt->channel_id = channel_id;
    pkt->type       = type;
    pkt->timestamp  = timestamp;
    pkt->extra      = 0;
    pkt->ts_field   = 0;

    return 0;
}