int hid_pointer_poll(HIDState *hs, uint8_t *buf, int len)
{
    int dx, dy, dz, l;
    int index;
    HIDPointerEvent *e;

    hs->idle_pending = false;

    hid_pointer_activate(hs);

    /* When the buffer is empty, return the last event.  Relative
       movements will all be zero.  */
    index = (hs->n ? hs->head : hs->head - 1);
    e = &hs->ptr.queue[index & QUEUE_MASK];

    if (hs->kind == HID_MOUSE) {
        dx = int_clamp(e->xdx, -127, 127);
        dy = int_clamp(e->ydy, -127, 127);
        e->xdx -= dx;
        e->ydy -= dy;
    } else {
        dx = e->xdx;
        dy = e->ydy;
    }
    dz = int_clamp(e->dz, -127, 127);
    e->dz -= dz;

    if (hs->n &&
        !e->dz &&
        (hs->kind == HID_TABLET || (!e->xdx && !e->ydy))) {
        /* that deals with this event */
        QUEUE_INCR(hs->head);
        hs->n--;
    }

    /* Appears we have to invert the wheel direction */
    dz = 0 - dz;
    l = 0;
    switch (hs->kind) {
    case HID_MOUSE:
        if (len > l) {
            buf[l++] = e->buttons_state;
        }
        if (len > l) {
            buf[l++] = dx;
        }
        if (len > l) {
            buf[l++] = dy;
        }
        if (len > l) {
            buf[l++] = dz;
        }
        break;

    case HID_TABLET:
        if (len > l) {
            buf[l++] = e->buttons_state;
        }
        if (len > l) {
            buf[l++] = dx & 0xff;
        }
        if (len > l) {
            buf[l++] = dx >> 8;
        }
        if (len > l) {
            buf[l++] = dy & 0xff;
        }
        if (len > l) {
            buf[l++] = dy >> 8;
        }
        if (len > l) {
            buf[l++] = dz;
        }
        break;

    default:
        abort();
    }

    return l;
}