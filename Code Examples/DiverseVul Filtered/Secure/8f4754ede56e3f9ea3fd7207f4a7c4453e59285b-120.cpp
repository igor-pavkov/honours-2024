void bdrv_drain_all(void)
{
    /* Always run first iteration so any pending completion BHs run */
    bool busy = true;
    BlockDriverState *bs;

    while (busy) {
        QTAILQ_FOREACH(bs, &bdrv_states, device_list) {
            bdrv_start_throttled_reqs(bs);
        }

        busy = bdrv_requests_pending_all();
        busy |= aio_poll(qemu_get_aio_context(), busy);
    }
}