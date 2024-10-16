void ide_atapi_cmd_error(IDEState *s, int sense_key, int asc)
{
    trace_ide_atapi_cmd_error(s, sense_key, asc);
    s->error = sense_key << 4;
    s->status = READY_STAT | ERR_STAT;
    s->nsector = (s->nsector & ~7) | ATAPI_INT_REASON_IO | ATAPI_INT_REASON_CD;
    s->sense_key = sense_key;
    s->asc = asc;
    ide_transfer_stop(s);
    ide_set_irq(s->bus);
}