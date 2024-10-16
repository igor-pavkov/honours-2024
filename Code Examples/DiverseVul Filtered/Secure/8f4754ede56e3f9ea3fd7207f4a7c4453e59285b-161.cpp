static int find_image_format(BlockDriverState *bs, const char *filename,
                             BlockDriver **pdrv, Error **errp)
{
    int score, score_max;
    BlockDriver *drv1, *drv;
    uint8_t buf[2048];
    int ret = 0;

    /* Return the raw BlockDriver * to scsi-generic devices or empty drives */
    if (bs->sg || !bdrv_is_inserted(bs) || bdrv_getlength(bs) == 0) {
        drv = bdrv_find_format("raw");
        if (!drv) {
            error_setg(errp, "Could not find raw image format");
            ret = -ENOENT;
        }
        *pdrv = drv;
        return ret;
    }

    ret = bdrv_pread(bs, 0, buf, sizeof(buf));
    if (ret < 0) {
        error_setg_errno(errp, -ret, "Could not read image for determining its "
                         "format");
        *pdrv = NULL;
        return ret;
    }

    score_max = 0;
    drv = NULL;
    QLIST_FOREACH(drv1, &bdrv_drivers, list) {
        if (drv1->bdrv_probe) {
            score = drv1->bdrv_probe(buf, ret, filename);
            if (score > score_max) {
                score_max = score;
                drv = drv1;
            }
        }
    }
    if (!drv) {
        error_setg(errp, "Could not determine image format: No compatible "
                   "driver found");
        ret = -ENOENT;
    }
    *pdrv = drv;
    return ret;
}