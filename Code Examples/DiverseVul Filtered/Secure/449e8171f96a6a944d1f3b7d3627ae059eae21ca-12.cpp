static int xattr_map_client(const struct lo_data *lo, const char *client_name,
                            char **out_name)
{
    size_t i;
    for (i = 0; i < lo->xattr_map_nentries; i++) {
        const XattrMapEntry *cur_entry = lo->xattr_map_list + i;

        if ((cur_entry->flags & XATTR_MAP_FLAG_CLIENT) &&
            (strstart(client_name, cur_entry->key, NULL))) {
            if (cur_entry->flags & XATTR_MAP_FLAG_BAD) {
                return -EPERM;
            }
            if (cur_entry->flags & XATTR_MAP_FLAG_UNSUPPORTED) {
                return -ENOTSUP;
            }
            if (cur_entry->flags & XATTR_MAP_FLAG_OK) {
                /* Unmodified name */
                return 0;
            }
            if (cur_entry->flags & XATTR_MAP_FLAG_PREFIX) {
                *out_name = g_try_malloc(strlen(client_name) +
                                         strlen(cur_entry->prepend) + 1);
                if (!*out_name) {
                    return -ENOMEM;
                }
                sprintf(*out_name, "%s%s", cur_entry->prepend, client_name);
                return 0;
            }
        }
    }

    return -EPERM;
}