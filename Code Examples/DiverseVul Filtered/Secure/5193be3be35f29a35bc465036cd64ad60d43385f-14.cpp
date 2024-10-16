void tsc210x_key_event(uWireSlave *chip, int key, int down)
{
    TSC210xState *s = (TSC210xState *) chip->opaque;

    if (down)
        s->kb.down |= 1 << key;
    else
        s->kb.down &= ~(1 << key);

    if (down && (s->kb.down & ~s->kb.mask) && !s->kb.intr) {
        s->kb.intr = 1;
        qemu_irq_lower(s->kbint);
    } else if (s->kb.intr && !(s->kb.down & ~s->kb.mask) &&
                    !(s->kb.mode & 1)) {
        s->kb.intr = 0;
        qemu_irq_raise(s->kbint);
    }
}