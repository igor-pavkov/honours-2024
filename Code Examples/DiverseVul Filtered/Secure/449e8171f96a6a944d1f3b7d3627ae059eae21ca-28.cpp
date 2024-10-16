static int xattr_map_server(const struct lo_data *lo, const char *server_name,
                            const char **out_name)
{
    size_t i;
    const char *end;

    for (i = 0; i < lo->xattr_map_nentries; i++) {
        const XattrMapEntry *cur_entry = lo->xattr_map_list + i;

        if ((cur_entry->flags & XATTR_MAP_FLAG_SERVER) &&
            (strstart(server_name, cur_entry->prepend, &end))) {
            if (cur_entry->flags & XATTR_MAP_FLAG_BAD ||
                cur_entry->flags & XATTR_MAP_FLAG_UNSUPPORTED) {
                return -ENODATA;
            }
            if (cur_entry->flags & XATTR_MAP_FLAG_OK) {
                *out_name = server_name;
                return 0;
            }
            if (cur_entry->flags & XATTR_MAP_FLAG_PREFIX) {
                /* Remove prefix */
                *out_name = end;
                return 0;
            }
        }
    }

    return -ENODATA;
}