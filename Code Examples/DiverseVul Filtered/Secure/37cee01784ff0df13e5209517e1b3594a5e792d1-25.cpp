static uint32_t rx_fifo_pop(lan9118_state *s)
{
    int n;
    uint32_t val;

    if (s->rxp_size == 0 && s->rxp_pad == 0) {
        s->rxp_size = s->rx_packet_size[s->rx_packet_size_head];
        s->rx_packet_size[s->rx_packet_size_head] = 0;
        if (s->rxp_size != 0) {
            s->rx_packet_size_head = (s->rx_packet_size_head + 1023) & 1023;
            s->rxp_offset = (s->rx_cfg >> 10) & 7;
            n = s->rxp_offset + s->rxp_size;
            switch (s->rx_cfg >> 30) {
            case 1:
                n = (-n) & 3;
                break;
            case 2:
                n = (-n) & 7;
                break;
            default:
                n = 0;
                break;
            }
            s->rxp_pad = n;
            DPRINTF("Pop packet size:%d offset:%d pad: %d\n",
                    s->rxp_size, s->rxp_offset, s->rxp_pad);
        }
    }
    if (s->rxp_offset > 0) {
        s->rxp_offset--;
        val = 0;
    } else if (s->rxp_size > 0) {
        s->rxp_size--;
        val = s->rx_fifo[s->rx_fifo_head++];
        if (s->rx_fifo_head >= s->rx_fifo_size) {
            s->rx_fifo_head -= s->rx_fifo_size;
        }
        s->rx_fifo_used--;
    } else if (s->rxp_pad > 0) {
        s->rxp_pad--;
        val =  0;
    } else {
        DPRINTF("RX underflow\n");
        s->int_sts |= RXE_INT;
        val =  0;
    }
    lan9118_update(s);
    return val;
}