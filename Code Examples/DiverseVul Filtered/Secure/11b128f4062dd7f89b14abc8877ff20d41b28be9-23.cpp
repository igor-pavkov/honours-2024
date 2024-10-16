int qcow2_mark_consistent(BlockDriverState *bs)
{
    BDRVQcowState *s = bs->opaque;

    if (s->incompatible_features & QCOW2_INCOMPAT_CORRUPT) {
        int ret = bdrv_flush(bs);
        if (ret < 0) {
            return ret;
        }

        s->incompatible_features &= ~QCOW2_INCOMPAT_CORRUPT;
        return qcow2_update_header(bs);
    }
    return 0;
}