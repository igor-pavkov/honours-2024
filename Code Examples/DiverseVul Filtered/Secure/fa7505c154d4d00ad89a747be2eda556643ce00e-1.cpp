static void do_cmd(ESPState *s)
{
    uint8_t busid = esp_fifo_pop(&s->cmdfifo);
    int len;

    s->cmdfifo_cdb_offset--;

    /* Ignore extended messages for now */
    if (s->cmdfifo_cdb_offset) {
        len = MIN(s->cmdfifo_cdb_offset, fifo8_num_used(&s->cmdfifo));
        esp_fifo_pop_buf(&s->cmdfifo, NULL, len);
        s->cmdfifo_cdb_offset = 0;
    }

    do_busid_cmd(s, busid);
}