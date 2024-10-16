static void cmd_start_stop_unit(IDEState *s, uint8_t* buf)
{
    int sense;
    bool start = buf[4] & 1;
    bool loej = buf[4] & 2;     /* load on start, eject on !start */
    int pwrcnd = buf[4] & 0xf0;

    if (pwrcnd) {
        /* eject/load only happens for power condition == 0 */
        ide_atapi_cmd_ok(s);
        return;
    }

    if (loej) {
        if (!start && !s->tray_open && s->tray_locked) {
            sense = blk_is_inserted(s->blk)
                ? NOT_READY : ILLEGAL_REQUEST;
            ide_atapi_cmd_error(s, sense, ASC_MEDIA_REMOVAL_PREVENTED);
            return;
        }

        if (s->tray_open != !start) {
            blk_eject(s->blk, !start);
            s->tray_open = !start;
        }
    }

    ide_atapi_cmd_ok(s);
}