int ff_alloc_packet(AVPacket *avpkt, int size)
{
    return ff_alloc_packet2(NULL, avpkt, size);
}