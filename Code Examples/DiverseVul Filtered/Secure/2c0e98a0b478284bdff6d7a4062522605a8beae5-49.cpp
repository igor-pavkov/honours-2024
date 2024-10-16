static int ascii_to_wc(AVIOContext *pb, const uint8_t *b)
{
    int val;
    while (*b) {
        GET_UTF8(val, *b++, return -1;)
        avio_wb16(pb, val);
    }
    avio_wb16(pb, 0x00);
    return 0;
}