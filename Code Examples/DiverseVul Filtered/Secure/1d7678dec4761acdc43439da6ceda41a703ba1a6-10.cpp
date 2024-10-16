static coroutine_fn int vhdx_co_readv(BlockDriverState *bs, int64_t sector_num,
                                      int nb_sectors, QEMUIOVector *qiov)
{
    BDRVVHDXState *s = bs->opaque;
    int ret = 0;
    VHDXSectorInfo sinfo;
    uint64_t bytes_done = 0;
    QEMUIOVector hd_qiov;

    qemu_iovec_init(&hd_qiov, qiov->niov);

    qemu_co_mutex_lock(&s->lock);

    while (nb_sectors > 0) {
        /* We are a differencing file, so we need to inspect the sector bitmap
         * to see if we have the data or not */
        if (s->params.data_bits & VHDX_PARAMS_HAS_PARENT) {
            /* not supported yet */
            ret = -ENOTSUP;
            goto exit;
        } else {
            vhdx_block_translate(s, sector_num, nb_sectors, &sinfo);

            qemu_iovec_reset(&hd_qiov);
            qemu_iovec_concat(&hd_qiov, qiov,  bytes_done, sinfo.bytes_avail);

            /* check the payload block state */
            switch (s->bat[sinfo.bat_idx] & VHDX_BAT_STATE_BIT_MASK) {
            case PAYLOAD_BLOCK_NOT_PRESENT: /* fall through */
            case PAYLOAD_BLOCK_UNDEFINED:   /* fall through */
            case PAYLOAD_BLOCK_UNMAPPED:    /* fall through */
            case PAYLOAD_BLOCK_ZERO:
                /* return zero */
                qemu_iovec_memset(&hd_qiov, 0, 0, sinfo.bytes_avail);
                break;
            case PAYLOAD_BLOCK_FULLY_PRESENT:
                qemu_co_mutex_unlock(&s->lock);
                ret = bdrv_co_readv(bs->file,
                                    sinfo.file_offset >> BDRV_SECTOR_BITS,
                                    sinfo.sectors_avail, &hd_qiov);
                qemu_co_mutex_lock(&s->lock);
                if (ret < 0) {
                    goto exit;
                }
                break;
            case PAYLOAD_BLOCK_PARTIALLY_PRESENT:
                /* we don't yet support difference files, fall through
                 * to error */
            default:
                ret = -EIO;
                goto exit;
                break;
            }
            nb_sectors -= sinfo.sectors_avail;
            sector_num += sinfo.sectors_avail;
            bytes_done += sinfo.bytes_avail;
        }
    }
    ret = 0;
exit:
    qemu_co_mutex_unlock(&s->lock);
    qemu_iovec_destroy(&hd_qiov);
    return ret;
}