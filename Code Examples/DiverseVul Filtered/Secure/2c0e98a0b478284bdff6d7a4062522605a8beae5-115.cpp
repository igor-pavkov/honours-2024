static int mov_parse_mpeg2_frame(AVPacket *pkt, uint32_t *flags)
{
    uint32_t c = -1;
    int i, closed_gop = 0;

    for (i = 0; i < pkt->size - 4; i++) {
        c = (c << 8) + pkt->data[i];
        if (c == 0x1b8) { // gop
            closed_gop = pkt->data[i + 4] >> 6 & 0x01;
        } else if (c == 0x100) { // pic
            int temp_ref = (pkt->data[i + 1] << 2) | (pkt->data[i + 2] >> 6);
            if (!temp_ref || closed_gop) // I picture is not reordered
                *flags = MOV_SYNC_SAMPLE;
            else
                *flags = MOV_PARTIAL_SYNC_SAMPLE;
            break;
        }
    }
    return 0;
}