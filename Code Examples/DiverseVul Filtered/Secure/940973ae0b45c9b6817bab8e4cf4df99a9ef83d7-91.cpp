void ide_exec_cmd(IDEBus *bus, uint32_t val)
{
    IDEState *s;
    bool complete;

#if defined(DEBUG_IDE)
    printf("ide: CMD=%02x\n", val);
#endif
    s = idebus_active_if(bus);
    /* ignore commands to non existent slave */
    if (s != bus->ifs && !s->bs)
        return;

    /* Only DEVICE RESET is allowed while BSY or/and DRQ are set */
    if ((s->status & (BUSY_STAT|DRQ_STAT)) && val != WIN_DEVICE_RESET)
        return;

    if (!ide_cmd_permitted(s, val)) {
        ide_abort_command(s);
        ide_set_irq(s->bus);
        return;
    }

    s->status = READY_STAT | BUSY_STAT;
    s->error = 0;

    complete = ide_cmd_table[val].handler(s, val);
    if (complete) {
        s->status &= ~BUSY_STAT;
        assert(!!s->error == !!(s->status & ERR_STAT));

        if ((ide_cmd_table[val].flags & SET_DSC) && !s->error) {
            s->status |= SEEK_STAT;
        }

        ide_set_irq(s->bus);
    }
}