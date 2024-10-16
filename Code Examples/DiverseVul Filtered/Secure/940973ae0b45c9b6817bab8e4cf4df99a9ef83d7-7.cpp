static bool cmd_set_multiple_mode(IDEState *s, uint8_t cmd)
{
    if (s->drive_kind == IDE_CFATA && s->nsector == 0) {
        /* Disable Read and Write Multiple */
        s->mult_sectors = 0;
    } else if ((s->nsector & 0xff) != 0 &&
        ((s->nsector & 0xff) > MAX_MULT_SECTORS ||
         (s->nsector & (s->nsector - 1)) != 0)) {
        ide_abort_command(s);
    } else {
        s->mult_sectors = s->nsector & 0xff;
    }

    return true;
}