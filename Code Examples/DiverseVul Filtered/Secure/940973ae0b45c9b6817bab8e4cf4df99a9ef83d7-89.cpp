static bool cmd_cfa_req_ext_error_code(IDEState *s, uint8_t cmd)
{
    s->error = 0x09;    /* miscellaneous error */
    s->status = READY_STAT | SEEK_STAT;
    ide_set_irq(s->bus);

    return false;
}