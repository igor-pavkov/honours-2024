static void write_response(ESPState *s)
{
    uint8_t buf[2];

    trace_esp_write_response(s->status);

    buf[0] = s->status;
    buf[1] = 0;

    if (s->dma) {
        if (s->dma_memory_write) {
            s->dma_memory_write(s->dma_opaque, buf, 2);
            s->rregs[ESP_RSTAT] = STAT_TC | STAT_ST;
            s->rregs[ESP_RINTR] |= INTR_BS | INTR_FC;
            s->rregs[ESP_RSEQ] = SEQ_CD;
        } else {
            s->pdma_cb = write_response_pdma_cb;
            esp_raise_drq(s);
            return;
        }
    } else {
        fifo8_reset(&s->fifo);
        fifo8_push_all(&s->fifo, buf, 2);
        s->rregs[ESP_RFLAGS] = 2;
    }
    esp_raise_irq(s);
}