static bool cmd_ibm_sense_condition(IDEState *s, uint8_t cmd)
{
    switch (s->feature) {
    case 0x01:  /* sense temperature in device */
        s->nsector = 0x50;      /* +20 C */
        break;
    default:
        ide_abort_command(s);
        return true;
    }

    return true;
}