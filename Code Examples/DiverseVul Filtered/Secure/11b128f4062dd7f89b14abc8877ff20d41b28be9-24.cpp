static int preallocate(BlockDriverState *bs)
{
    uint64_t nb_sectors;
    uint64_t offset;
    uint64_t host_offset = 0;
    int num;
    int ret;
    QCowL2Meta *meta;

    nb_sectors = bdrv_getlength(bs) >> BDRV_SECTOR_BITS;
    offset = 0;

    while (nb_sectors) {
        num = MIN(nb_sectors, INT_MAX >> BDRV_SECTOR_BITS);
        ret = qcow2_alloc_cluster_offset(bs, offset, &num,
                                         &host_offset, &meta);
        if (ret < 0) {
            return ret;
        }

        if (meta != NULL) {
            ret = qcow2_alloc_cluster_link_l2(bs, meta);
            if (ret < 0) {
                qcow2_free_any_clusters(bs, meta->alloc_offset,
                                        meta->nb_clusters, QCOW2_DISCARD_NEVER);
                return ret;
            }

            /* There are no dependent requests, but we need to remove our
             * request from the list of in-flight requests */
            QLIST_REMOVE(meta, next_in_flight);
        }

        /* TODO Preallocate data if requested */

        nb_sectors -= num;
        offset += num << BDRV_SECTOR_BITS;
    }

    /*
     * It is expected that the image file is large enough to actually contain
     * all of the allocated clusters (otherwise we get failing reads after
     * EOF). Extend the image to the last allocated sector.
     */
    if (host_offset != 0) {
        uint8_t buf[BDRV_SECTOR_SIZE];
        memset(buf, 0, BDRV_SECTOR_SIZE);
        ret = bdrv_write(bs->file, (host_offset >> BDRV_SECTOR_BITS) + num - 1,
                         buf, 1);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}