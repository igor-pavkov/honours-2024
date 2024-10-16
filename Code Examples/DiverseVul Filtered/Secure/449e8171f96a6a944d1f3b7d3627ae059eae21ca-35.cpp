static bool block_xattr(struct lo_data *lo, const char *name)
{
    /*
     * If user explicitly enabled posix_acl or did not provide any option,
     * do not block acl. Otherwise block system.posix_acl_access and
     * system.posix_acl_default xattrs.
     */
    if (lo->user_posix_acl) {
        return false;
    }
    if (!strcmp(name, "system.posix_acl_access") ||
        !strcmp(name, "system.posix_acl_default"))
            return true;

    return false;
}