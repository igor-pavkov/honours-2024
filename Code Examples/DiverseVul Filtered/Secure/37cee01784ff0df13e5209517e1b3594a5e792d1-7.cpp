static uint32_t rx_status_fifo_pop(lan9118_state *s)
{
    uint32_t val;

    val = s->rx_status_fifo[s->rx_status_fifo_head];
    if (s->rx_status_fifo_used != 0) {
        s->rx_status_fifo_used--;
        s->rx_status_fifo_head++;
        if (s->rx_status_fifo_head >= s->rx_status_fifo_size) {
            s->rx_status_fifo_head -= s->rx_status_fifo_size;
        }
        /* ??? What value should be returned when the FIFO is empty?  */
        DPRINTF("RX status pop 0x%08x\n", val);
    }
    return val;
}