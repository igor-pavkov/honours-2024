static void coroutine_fn mirror_iteration_done(MirrorOp *op, int ret)
{
    MirrorBlockJob *s = op->s;
    struct iovec *iov;
    int64_t chunk_num;
    int i, nb_chunks;

    trace_mirror_iteration_done(s, op->offset, op->bytes, ret);

    s->in_flight--;
    s->bytes_in_flight -= op->bytes;
    iov = op->qiov.iov;
    for (i = 0; i < op->qiov.niov; i++) {
        MirrorBuffer *buf = (MirrorBuffer *) iov[i].iov_base;
        QSIMPLEQ_INSERT_TAIL(&s->buf_free, buf, next);
        s->buf_free_count++;
    }

    chunk_num = op->offset / s->granularity;
    nb_chunks = DIV_ROUND_UP(op->bytes, s->granularity);

    bitmap_clear(s->in_flight_bitmap, chunk_num, nb_chunks);
    QTAILQ_REMOVE(&s->ops_in_flight, op, next);
    if (ret >= 0) {
        if (s->cow_bitmap) {
            bitmap_set(s->cow_bitmap, chunk_num, nb_chunks);
        }
        if (!s->initial_zeroing_ongoing) {
            job_progress_update(&s->common.job, op->bytes);
        }
    }
    qemu_iovec_destroy(&op->qiov);

    qemu_co_queue_restart_all(&op->waiting_requests);
    g_free(op);
}