static void coroutine_fn mirror_co_read(void *opaque)
{
    MirrorOp *op = opaque;
    MirrorBlockJob *s = op->s;
    int nb_chunks;
    uint64_t ret;
    uint64_t max_bytes;

    max_bytes = s->granularity * s->max_iov;

    /* We can only handle as much as buf_size at a time. */
    op->bytes = MIN(s->buf_size, MIN(max_bytes, op->bytes));
    assert(op->bytes);
    assert(op->bytes < BDRV_REQUEST_MAX_BYTES);
    *op->bytes_handled = op->bytes;

    if (s->cow_bitmap) {
        *op->bytes_handled += mirror_cow_align(s, &op->offset, &op->bytes);
    }
    /* Cannot exceed BDRV_REQUEST_MAX_BYTES + INT_MAX */
    assert(*op->bytes_handled <= UINT_MAX);
    assert(op->bytes <= s->buf_size);
    /* The offset is granularity-aligned because:
     * 1) Caller passes in aligned values;
     * 2) mirror_cow_align is used only when target cluster is larger. */
    assert(QEMU_IS_ALIGNED(op->offset, s->granularity));
    /* The range is sector-aligned, since bdrv_getlength() rounds up. */
    assert(QEMU_IS_ALIGNED(op->bytes, BDRV_SECTOR_SIZE));
    nb_chunks = DIV_ROUND_UP(op->bytes, s->granularity);

    while (s->buf_free_count < nb_chunks) {
        trace_mirror_yield_in_flight(s, op->offset, s->in_flight);
        mirror_wait_for_free_in_flight_slot(s);
    }

    /* Now make a QEMUIOVector taking enough granularity-sized chunks
     * from s->buf_free.
     */
    qemu_iovec_init(&op->qiov, nb_chunks);
    while (nb_chunks-- > 0) {
        MirrorBuffer *buf = QSIMPLEQ_FIRST(&s->buf_free);
        size_t remaining = op->bytes - op->qiov.size;

        QSIMPLEQ_REMOVE_HEAD(&s->buf_free, next);
        s->buf_free_count--;
        qemu_iovec_add(&op->qiov, buf, MIN(s->granularity, remaining));
    }

    /* Copy the dirty cluster.  */
    s->in_flight++;
    s->bytes_in_flight += op->bytes;
    op->is_in_flight = true;
    trace_mirror_one_iteration(s, op->offset, op->bytes);

    ret = bdrv_co_preadv(s->mirror_top_bs->backing, op->offset, op->bytes,
                         &op->qiov, 0);
    mirror_read_complete(op, ret);
}