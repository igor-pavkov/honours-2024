static MirrorOp *coroutine_fn active_write_prepare(MirrorBlockJob *s,
                                                   uint64_t offset,
                                                   uint64_t bytes)
{
    MirrorOp *op;
    uint64_t start_chunk = offset / s->granularity;
    uint64_t end_chunk = DIV_ROUND_UP(offset + bytes, s->granularity);

    op = g_new(MirrorOp, 1);
    *op = (MirrorOp){
        .s                  = s,
        .offset             = offset,
        .bytes              = bytes,
        .is_active_write    = true,
        .is_in_flight       = true,
        .co                 = qemu_coroutine_self(),
    };
    qemu_co_queue_init(&op->waiting_requests);
    QTAILQ_INSERT_TAIL(&s->ops_in_flight, op, next);

    s->in_active_write_counter++;

    mirror_wait_on_conflicts(op, s, offset, bytes);

    bitmap_set(s->in_flight_bitmap, start_chunk, end_chunk - start_chunk);

    return op;
}