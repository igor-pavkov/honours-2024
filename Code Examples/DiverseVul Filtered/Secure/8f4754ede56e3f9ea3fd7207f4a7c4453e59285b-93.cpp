bool bdrv_is_first_non_filter(BlockDriverState *candidate)
{
    BlockDriverState *bs;

    /* walk down the bs forest recursively */
    QTAILQ_FOREACH(bs, &bdrv_states, device_list) {
        bool perm;

        /* try to recurse in this top level bs */
        perm = bdrv_recurse_is_first_non_filter(bs, candidate);

        /* candidate is the first non filter */
        if (perm) {
            return true;
        }
    }

    return false;
}