static int cd_read_sector(IDEState *s)
{
    void *buf;

    if (s->cd_sector_size != 2048 && s->cd_sector_size != 2352) {
        block_acct_invalid(blk_get_stats(s->blk), BLOCK_ACCT_READ);
        return -EINVAL;
    }

    buf = (s->cd_sector_size == 2352) ? s->io_buffer + 16 : s->io_buffer;
    qemu_iovec_init_buf(&s->qiov, buf, ATAPI_SECTOR_SIZE);

    trace_cd_read_sector(s->lba);

    block_acct_start(blk_get_stats(s->blk), &s->acct,
                     ATAPI_SECTOR_SIZE, BLOCK_ACCT_READ);

    ide_buffered_readv(s, (int64_t)s->lba << 2, &s->qiov, 4,
                       cd_read_sector_cb, s);

    s->status |= BUSY_STAT;
    return 0;
}