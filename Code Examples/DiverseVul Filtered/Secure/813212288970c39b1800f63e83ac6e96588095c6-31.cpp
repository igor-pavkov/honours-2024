static void cd_read_sector_cb(void *opaque, int ret)
{
    IDEState *s = opaque;

    trace_cd_read_sector_cb(s->lba, ret);

    if (ret < 0) {
        block_acct_failed(blk_get_stats(s->blk), &s->acct);
        ide_atapi_io_error(s, ret);
        return;
    }

    block_acct_done(blk_get_stats(s->blk), &s->acct);

    if (s->cd_sector_size == 2352) {
        cd_data_to_raw(s->io_buffer, s->lba);
    }

    s->lba++;
    s->io_buffer_index = 0;
    s->status &= ~BUSY_STAT;

    ide_atapi_cmd_reply_end(s);
}