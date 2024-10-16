static guint8 compute_crc8(tvbuff_t *p, guint8 len, guint8 offset)
{
    int    i;
    guint8 crc = 0, tmp;

    for (i = 0; i < len; i++) {
        tmp = tvb_get_guint8(p, offset++);
        crc = crc8_table[crc ^ tmp];
    }
    return crc;
}