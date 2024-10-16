static int hid_post_load(void *opaque, int version_id)
{
    HIDState *s = opaque;

    hid_set_next_idle(s);

    if (s->n == QUEUE_LENGTH && (s->kind == HID_TABLET ||
                                 s->kind == HID_MOUSE)) {
        /*
         * Handle ptr device migration from old qemu with full queue.
         *
         * Throw away everything but the last event, so we propagate
         * at least the current button state to the guest.  Also keep
         * current position for the tablet, signal "no motion" for the
         * mouse.
         */
        HIDPointerEvent evt;
        evt = s->ptr.queue[(s->head+s->n) & QUEUE_MASK];
        if (s->kind == HID_MOUSE) {
            evt.xdx = 0;
            evt.ydy = 0;
        }
        s->ptr.queue[0] = evt;
        s->head = 0;
        s->n = 1;
    }
    return 0;
}