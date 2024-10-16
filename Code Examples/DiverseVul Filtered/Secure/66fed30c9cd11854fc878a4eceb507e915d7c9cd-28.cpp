static unsigned mirror_perform(MirrorBlockJob *s, int64_t offset,
                               unsigned bytes, MirrorMethod mirror_method)
{
    MirrorOp *op;
    Coroutine *co;
    int64_t bytes_handled = -1;

    op = g_new(MirrorOp, 1);
    *op = (MirrorOp){
        .s              = s,
        .offset         = offset,
        .bytes          = bytes,
        .bytes_handled  = &bytes_handled,
    };
    qemu_co_queue_init(&op->waiting_requests);

    switch (mirror_method) {
    case MIRROR_METHOD_COPY:
        co = qemu_coroutine_create(mirror_co_read, op);
        break;
    case MIRROR_METHOD_ZERO:
        co = qemu_coroutine_create(mirror_co_zero, op);
        break;
    case MIRROR_METHOD_DISCARD:
        co = qemu_coroutine_create(mirror_co_discard, op);
        break;
    default:
        abort();
    }
    op->co = co;

    QTAILQ_INSERT_TAIL(&s->ops_in_flight, op, next);
    qemu_coroutine_enter(co);
    /* At this point, ownership of op has been moved to the coroutine
     * and the object may already be freed */

    /* Assert that this value has been set */
    assert(bytes_handled >= 0);

    /* Same assertion as in mirror_co_read() (and for mirror_co_read()
     * and mirror_co_discard(), bytes_handled == op->bytes, which
     * is the @bytes parameter given to this function) */
    assert(bytes_handled <= UINT_MAX);
    return bytes_handled;
}