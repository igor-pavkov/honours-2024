static uint32_t tx_status_fifo_pop(lan9118_state *s)
{
    uint32_t val;

    val = s->tx_status_fifo[s->tx_status_fifo_head];
    if (s->tx_status_fifo_used != 0) {
        s->tx_status_fifo_used--;
        s->tx_status_fifo_head = (s->tx_status_fifo_head + 1) & 511;
        /* ??? What value should be returned when the FIFO is empty?  */
    }
    return val;
}