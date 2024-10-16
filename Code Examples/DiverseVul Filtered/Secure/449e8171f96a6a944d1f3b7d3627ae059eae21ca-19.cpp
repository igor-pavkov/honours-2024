static int remove_blocked_xattrs(struct lo_data *lo, char *xattr_list,
                                 unsigned in_size)
{
    size_t out_index, in_index;

    /*
     * As of now we only filter out acl xattrs. If acls are enabled or
     * they have not been explicitly disabled, there is nothing to
     * filter.
     */
    if (lo->user_posix_acl) {
        return in_size;
    }

    out_index = 0;
    in_index = 0;
    while (in_index < in_size) {
        char *in_ptr = xattr_list + in_index;

        /* Length of current attribute name */
        size_t in_len = strlen(xattr_list + in_index) + 1;

        if (!block_xattr(lo, in_ptr)) {
            if (in_index != out_index) {
                memmove(xattr_list + out_index, xattr_list + in_index, in_len);
            }
            out_index += in_len;
        }
        in_index += in_len;
     }
    return out_index;
}