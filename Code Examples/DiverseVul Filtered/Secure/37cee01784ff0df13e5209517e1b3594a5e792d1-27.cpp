static void rx_fifo_push(lan9118_state *s, uint32_t val)
{
    int fifo_pos;
    fifo_pos = s->rx_fifo_head + s->rx_fifo_used;
    if (fifo_pos >= s->rx_fifo_size)
      fifo_pos -= s->rx_fifo_size;
    s->rx_fifo[fifo_pos] = val;
    s->rx_fifo_used++;
}