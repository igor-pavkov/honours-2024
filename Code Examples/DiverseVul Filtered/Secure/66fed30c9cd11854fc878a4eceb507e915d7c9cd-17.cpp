static void coroutine_fn mirror_co_discard(void *opaque)
{
    MirrorOp *op = opaque;
    int ret;

    op->s->in_flight++;
    op->s->bytes_in_flight += op->bytes;
    *op->bytes_handled = op->bytes;
    op->is_in_flight = true;

    ret = blk_co_pdiscard(op->s->target, op->offset, op->bytes);
    mirror_write_complete(op, ret);
}