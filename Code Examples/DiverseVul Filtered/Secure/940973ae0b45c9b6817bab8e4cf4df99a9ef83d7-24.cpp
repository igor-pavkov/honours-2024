static bool cmd_exec_dev_diagnostic(IDEState *s, uint8_t cmd)
{
    ide_set_signature(s);

    if (s->drive_kind == IDE_CD) {
        s->status = 0; /* ATAPI spec (v6) section 9.10 defines packet
                        * devices to return a clear status register
                        * with READY_STAT *not* set. */
        s->error = 0x01;
    } else {
        s->status = READY_STAT | SEEK_STAT;
        /* The bits of the error register are not as usual for this command!
         * They are part of the regular output (this is why ERR_STAT isn't set)
         * Device 0 passed, Device 1 passed or not present. */
        s->error = 0x01;
        ide_set_irq(s->bus);
    }

    return false;
}