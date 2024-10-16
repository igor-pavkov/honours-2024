int bdrv_make_zero(BlockDriverState *bs, BdrvRequestFlags flags)
{
    int64_t target_size = bdrv_getlength(bs) / BDRV_SECTOR_SIZE;
    int64_t ret, nb_sectors, sector_num = 0;
    int n;

    for (;;) {
        nb_sectors = target_size - sector_num;
        if (nb_sectors <= 0) {
            return 0;
        }
        if (nb_sectors > INT_MAX) {
            nb_sectors = INT_MAX;
        }
        ret = bdrv_get_block_status(bs, sector_num, nb_sectors, &n);
        if (ret < 0) {
            error_report("error getting block status at sector %" PRId64 ": %s",
                         sector_num, strerror(-ret));
            return ret;
        }
        if (ret & BDRV_BLOCK_ZERO) {
            sector_num += n;
            continue;
        }
        ret = bdrv_write_zeroes(bs, sector_num, n, flags);
        if (ret < 0) {
            error_report("error writing zeroes at sector %" PRId64 ": %s",
                         sector_num, strerror(-ret));
            return ret;
        }
        sector_num += n;
    }
}