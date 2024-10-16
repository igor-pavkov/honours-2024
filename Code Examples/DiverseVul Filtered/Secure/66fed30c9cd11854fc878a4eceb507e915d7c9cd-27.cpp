static void coroutine_fn mirror_co_zero(void *opaque)
{
    MirrorOp *op = opaque;
    int ret;

    op->s->in_flight++;
    op->s->bytes_in_flight += op->bytes;
    *op->bytes_handled = op->bytes;
    op->is_in_flight = true;

    ret = blk_co_pwrite_zeroes(op->s->target, op->offset, op->bytes,
                               op->s->unmap ? BDRV_REQ_MAY_UNMAP : 0);
    mirror_write_complete(op, ret);
}