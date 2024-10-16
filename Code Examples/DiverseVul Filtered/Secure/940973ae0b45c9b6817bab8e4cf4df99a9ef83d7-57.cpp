static void ide_flush_cb(void *opaque, int ret)
{
    IDEState *s = opaque;

    if (ret < 0) {
        /* XXX: What sector number to set here? */
        if (ide_handle_rw_error(s, -ret, BM_STATUS_RETRY_FLUSH)) {
            return;
        }
    }

    bdrv_acct_done(s->bs, &s->acct);
    s->status = READY_STAT | SEEK_STAT;
    ide_async_cmd_done(s);
    ide_set_irq(s->bus);
}