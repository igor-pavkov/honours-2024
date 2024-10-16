static bool ide_tray_state_needed(void *opaque)
{
    IDEState *s = opaque;

    return s->tray_open || s->tray_locked;
}