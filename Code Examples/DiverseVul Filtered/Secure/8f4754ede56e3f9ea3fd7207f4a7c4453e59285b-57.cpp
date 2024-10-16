void bdrv_close(BlockDriverState *bs)
{
    if (bs->job) {
        block_job_cancel_sync(bs->job);
    }
    bdrv_drain_all(); /* complete I/O */
    bdrv_flush(bs);
    bdrv_drain_all(); /* in case flush left pending I/O */
    notifier_list_notify(&bs->close_notifiers, bs);

    if (bs->drv) {
        if (bs->backing_hd) {
            bdrv_unref(bs->backing_hd);
            bs->backing_hd = NULL;
        }
        bs->drv->bdrv_close(bs);
        g_free(bs->opaque);
#ifdef _WIN32
        if (bs->is_temporary) {
            unlink(bs->filename);
        }
#endif
        bs->opaque = NULL;
        bs->drv = NULL;
        bs->copy_on_read = 0;
        bs->backing_file[0] = '\0';
        bs->backing_format[0] = '\0';
        bs->total_sectors = 0;
        bs->encrypted = 0;
        bs->valid_key = 0;
        bs->sg = 0;
        bs->growable = 0;
        bs->zero_beyond_eof = false;
        QDECREF(bs->options);
        bs->options = NULL;

        if (bs->file != NULL) {
            bdrv_unref(bs->file);
            bs->file = NULL;
        }
    }

    bdrv_dev_change_media_cb(bs, false);

    /*throttling disk I/O limits*/
    if (bs->io_limits_enabled) {
        bdrv_io_limits_disable(bs);
    }
}