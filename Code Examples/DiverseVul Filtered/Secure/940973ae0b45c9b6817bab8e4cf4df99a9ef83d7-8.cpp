static bool cmd_device_reset(IDEState *s, uint8_t cmd)
{
    ide_set_signature(s);
    s->status = 0x00; /* NOTE: READY is _not_ set */
    s->error = 0x01;

    return false;
}