static void coroutine_fn active_write_settle(MirrorOp *op)
{
    uint64_t start_chunk = op->offset / op->s->granularity;
    uint64_t end_chunk = DIV_ROUND_UP(op->offset + op->bytes,
                                      op->s->granularity);

    if (!--op->s->in_active_write_counter && op->s->actively_synced) {
        BdrvChild *source = op->s->mirror_top_bs->backing;

        if (QLIST_FIRST(&source->bs->parents) == source &&
            QLIST_NEXT(source, next_parent) == NULL)
        {
            /* Assert that we are back in sync once all active write
             * operations are settled.
             * Note that we can only assert this if the mirror node
             * is the source node's only parent. */
            assert(!bdrv_get_dirty_count(op->s->dirty_bitmap));
        }
    }
    bitmap_clear(op->s->in_flight_bitmap, start_chunk, end_chunk - start_chunk);
    QTAILQ_REMOVE(&op->s->ops_in_flight, op, next);
    qemu_co_queue_restart_all(&op->waiting_requests);
    g_free(op);
}