int qcow2_mark_corrupt(BlockDriverState *bs)
{
    BDRVQcowState *s = bs->opaque;

    s->incompatible_features |= QCOW2_INCOMPAT_CORRUPT;
    return qcow2_update_header(bs);
}