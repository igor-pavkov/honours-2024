static coroutine_fn int vhdx_co_writev(BlockDriverState *bs, int64_t sector_num,
                                      int nb_sectors, QEMUIOVector *qiov)
{
    int ret = -ENOTSUP;
    BDRVVHDXState *s = bs->opaque;
    VHDXSectorInfo sinfo;
    uint64_t bytes_done = 0;
    uint64_t bat_entry = 0;
    uint64_t bat_entry_offset = 0;
    QEMUIOVector hd_qiov;
    struct iovec iov1 = { 0 };
    struct iovec iov2 = { 0 };
    int sectors_to_write;
    int bat_state;
    uint64_t bat_prior_offset = 0;
    bool bat_update = false;

    qemu_iovec_init(&hd_qiov, qiov->niov);

    qemu_co_mutex_lock(&s->lock);

    ret = vhdx_user_visible_write(bs, s);
    if (ret < 0) {
        goto exit;
    }

    while (nb_sectors > 0) {
        bool use_zero_buffers = false;
        bat_update = false;
        if (s->params.data_bits & VHDX_PARAMS_HAS_PARENT) {
            /* not supported yet */
            ret = -ENOTSUP;
            goto exit;
        } else {
            vhdx_block_translate(s, sector_num, nb_sectors, &sinfo);
            sectors_to_write = sinfo.sectors_avail;

            qemu_iovec_reset(&hd_qiov);
            /* check the payload block state */
            bat_state = s->bat[sinfo.bat_idx] & VHDX_BAT_STATE_BIT_MASK;
            switch (bat_state) {
            case PAYLOAD_BLOCK_ZERO:
                /* in this case, we need to preserve zero writes for
                 * data that is not part of this write, so we must pad
                 * the rest of the buffer to zeroes */

                /* if we are on a posix system with ftruncate() that extends
                 * a file, then it is zero-filled for us.  On Win32, the raw
                 * layer uses SetFilePointer and SetFileEnd, which does not
                 * zero fill AFAIK */

                /* Queue another write of zero buffers if the underlying file
                 * does not zero-fill on file extension */

                if (bdrv_has_zero_init(bs->file) == 0) {
                    use_zero_buffers = true;

                    /* zero fill the front, if any */
                    if (sinfo.block_offset) {
                        iov1.iov_len = sinfo.block_offset;
                        iov1.iov_base = qemu_blockalign(bs, iov1.iov_len);
                        memset(iov1.iov_base, 0, iov1.iov_len);
                        qemu_iovec_concat_iov(&hd_qiov, &iov1, 1, 0,
                                              sinfo.block_offset);
                        sectors_to_write += iov1.iov_len >> BDRV_SECTOR_BITS;
                    }

                    /* our actual data */
                    qemu_iovec_concat(&hd_qiov, qiov,  bytes_done,
                                      sinfo.bytes_avail);

                    /* zero fill the back, if any */
                    if ((sinfo.bytes_avail - sinfo.block_offset) <
                         s->block_size) {
                        iov2.iov_len = s->block_size -
                                      (sinfo.bytes_avail + sinfo.block_offset);
                        iov2.iov_base = qemu_blockalign(bs, iov2.iov_len);
                        memset(iov2.iov_base, 0, iov2.iov_len);
                        qemu_iovec_concat_iov(&hd_qiov, &iov2, 1, 0,
                                              sinfo.block_offset);
                        sectors_to_write += iov2.iov_len >> BDRV_SECTOR_BITS;
                    }
                }

                /* fall through */
            case PAYLOAD_BLOCK_NOT_PRESENT: /* fall through */
            case PAYLOAD_BLOCK_UNMAPPED:    /* fall through */
            case PAYLOAD_BLOCK_UNDEFINED:   /* fall through */
                bat_prior_offset = sinfo.file_offset;
                ret = vhdx_allocate_block(bs, s, &sinfo.file_offset);
                if (ret < 0) {
                    goto exit;
                }
                /* once we support differencing files, this may also be
                 * partially present */
                /* update block state to the newly specified state */
                vhdx_update_bat_table_entry(bs, s, &sinfo, &bat_entry,
                                            &bat_entry_offset,
                                            PAYLOAD_BLOCK_FULLY_PRESENT);
                bat_update = true;
                /* since we just allocated a block, file_offset is the
                 * beginning of the payload block. It needs to be the
                 * write address, which includes the offset into the block */
                if (!use_zero_buffers) {
                    sinfo.file_offset += sinfo.block_offset;
                }
                /* fall through */
            case PAYLOAD_BLOCK_FULLY_PRESENT:
                /* if the file offset address is in the header zone,
                 * there is a problem */
                if (sinfo.file_offset < (1024 * 1024)) {
                    ret = -EFAULT;
                    goto error_bat_restore;
                }

                if (!use_zero_buffers) {
                    qemu_iovec_concat(&hd_qiov, qiov,  bytes_done,
                                      sinfo.bytes_avail);
                }
                /* block exists, so we can just overwrite it */
                qemu_co_mutex_unlock(&s->lock);
                ret = bdrv_co_writev(bs->file,
                                    sinfo.file_offset >> BDRV_SECTOR_BITS,
                                    sectors_to_write, &hd_qiov);
                qemu_co_mutex_lock(&s->lock);
                if (ret < 0) {
                    goto error_bat_restore;
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

            if (bat_update) {
                /* this will update the BAT entry into the log journal, and
                 * then flush the log journal out to disk */
                ret =  vhdx_log_write_and_flush(bs, s, &bat_entry,
                                                sizeof(VHDXBatEntry),
                                                bat_entry_offset);
                if (ret < 0) {
                    goto exit;
                }
            }

            nb_sectors -= sinfo.sectors_avail;
            sector_num += sinfo.sectors_avail;
            bytes_done += sinfo.bytes_avail;

        }
    }

    goto exit;

error_bat_restore:
    if (bat_update) {
        /* keep metadata in sync, and restore the bat entry state
         * if error. */
        sinfo.file_offset = bat_prior_offset;
        vhdx_update_bat_table_entry(bs, s, &sinfo, &bat_entry,
                                    &bat_entry_offset, bat_state);
    }
exit:
    qemu_vfree(iov1.iov_base);
    qemu_vfree(iov2.iov_base);
    qemu_co_mutex_unlock(&s->lock);
    qemu_iovec_destroy(&hd_qiov);
    return ret;
}