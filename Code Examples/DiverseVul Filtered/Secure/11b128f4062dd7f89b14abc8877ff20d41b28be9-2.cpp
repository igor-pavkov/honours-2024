static int qcow2_downgrade(BlockDriverState *bs, int target_version)
{
    BDRVQcowState *s = bs->opaque;
    int current_version = s->qcow_version;
    int ret;

    if (target_version == current_version) {
        return 0;
    } else if (target_version > current_version) {
        return -EINVAL;
    } else if (target_version != 2) {
        return -EINVAL;
    }

    if (s->refcount_order != 4) {
        /* we would have to convert the image to a refcount_order == 4 image
         * here; however, since qemu (at the time of writing this) does not
         * support anything different than 4 anyway, there is no point in doing
         * so right now; however, we should error out (if qemu supports this in
         * the future and this code has not been adapted) */
        error_report("qcow2_downgrade: Image refcount orders other than 4 are "
                     "currently not supported.");
        return -ENOTSUP;
    }

    /* clear incompatible features */
    if (s->incompatible_features & QCOW2_INCOMPAT_DIRTY) {
        ret = qcow2_mark_clean(bs);
        if (ret < 0) {
            return ret;
        }
    }

    /* with QCOW2_INCOMPAT_CORRUPT, it is pretty much impossible to get here in
     * the first place; if that happens nonetheless, returning -ENOTSUP is the
     * best thing to do anyway */

    if (s->incompatible_features) {
        return -ENOTSUP;
    }

    /* since we can ignore compatible features, we can set them to 0 as well */
    s->compatible_features = 0;
    /* if lazy refcounts have been used, they have already been fixed through
     * clearing the dirty flag */

    /* clearing autoclear features is trivial */
    s->autoclear_features = 0;

    ret = qcow2_expand_zero_clusters(bs);
    if (ret < 0) {
        return ret;
    }

    s->qcow_version = target_version;
    ret = qcow2_update_header(bs);
    if (ret < 0) {
        s->qcow_version = current_version;
        return ret;
    }
    return 0;
}