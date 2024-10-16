static ssize_t lan9118_receive(NetClientState *nc, const uint8_t *buf,
                               size_t size)
{
    lan9118_state *s = qemu_get_nic_opaque(nc);
    int fifo_len;
    int offset;
    int src_pos;
    int n;
    int filter;
    uint32_t val;
    uint32_t crc;
    uint32_t status;

    if ((s->mac_cr & MAC_CR_RXEN) == 0) {
        return -1;
    }

    if (size >= 2048 || size < 14) {
        return -1;
    }

    /* TODO: Implement FIFO overflow notification.  */
    if (s->rx_status_fifo_used == s->rx_status_fifo_size) {
        return -1;
    }

    filter = lan9118_filter(s, buf);
    if (!filter && (s->mac_cr & MAC_CR_RXALL) == 0) {
        return size;
    }

    offset = (s->rx_cfg >> 8) & 0x1f;
    n = offset & 3;
    fifo_len = (size + n + 3) >> 2;
    /* Add a word for the CRC.  */
    fifo_len++;
    if (s->rx_fifo_size - s->rx_fifo_used < fifo_len) {
        return -1;
    }

    DPRINTF("Got packet len:%d fifo:%d filter:%s\n",
            (int)size, fifo_len, filter ? "pass" : "fail");
    val = 0;
    crc = bswap32(crc32(~0, buf, size));
    for (src_pos = 0; src_pos < size; src_pos++) {
        val = (val >> 8) | ((uint32_t)buf[src_pos] << 24);
        n++;
        if (n == 4) {
            n = 0;
            rx_fifo_push(s, val);
            val = 0;
        }
    }
    if (n) {
        val >>= ((4 - n) * 8);
        val |= crc << (n * 8);
        rx_fifo_push(s, val);
        val = crc >> ((4 - n) * 8);
        rx_fifo_push(s, val);
    } else {
        rx_fifo_push(s, crc);
    }
    n = s->rx_status_fifo_head + s->rx_status_fifo_used;
    if (n >= s->rx_status_fifo_size) {
        n -= s->rx_status_fifo_size;
    }
    s->rx_packet_size[s->rx_packet_size_tail] = fifo_len;
    s->rx_packet_size_tail = (s->rx_packet_size_tail + 1023) & 1023;
    s->rx_status_fifo_used++;

    status = (size + 4) << 16;
    if (buf[0] == 0xff && buf[1] == 0xff && buf[2] == 0xff &&
        buf[3] == 0xff && buf[4] == 0xff && buf[5] == 0xff) {
        status |= 0x00002000;
    } else if (buf[0] & 1) {
        status |= 0x00000400;
    }
    if (!filter) {
        status |= 0x40000000;
    }
    s->rx_status_fifo[n] = status;

    if (s->rx_status_fifo_used > (s->fifo_int & 0xff)) {
        s->int_sts |= RSFL_INT;
    }
    lan9118_update(s);

    return size;
}