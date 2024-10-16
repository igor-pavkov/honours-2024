int ul_check_free_space(const char *name, const double min_space)
{
    STATFS_STRUCT statfsbuf;
    char *z;
    char *alloca_namedir;
    size_t name_len;
    double jam;
    double space;

    if (maxdiskusagepct <= 0.0 && min_space <= 0.0) {
        return 1;
    }
#ifdef CHECK_SYMLINKS_DISK_SPACE
    if (STATFS(name, &statfsbuf) == 0) {
        goto okcheckspace;
    }
#endif
    name_len = strlen(name) + (size_t) 1U;
    if (name_len < (size_t) 2U ||
        (alloca_namedir = ALLOCA(name_len)) == NULL) {
        return -1;
    }
    memcpy(alloca_namedir, name, name_len);
    if ((z = strrchr(alloca_namedir, '/')) != NULL) {
        if (z == alloca_namedir) {
            *z++ = '.';
        }
        *z = 0;
    } else {
        alloca_namedir[0] = '.';
        alloca_namedir[1] = 0;
    }
    if (STATFS(alloca_namedir, &statfsbuf) != 0) {
        ALLOCA_FREE(alloca_namedir);
        return -1;
    }
    ALLOCA_FREE(alloca_namedir);

#ifdef CHECK_SYMLINKS_DISK_SPACE
    okcheckspace:
#endif
    if ((double) STATFS_BLOCKS(statfsbuf) <= 0.0) {
        return 1;
    }
    if (min_space >= 0.0) {
        space = (double) STATFS_BAVAIL(statfsbuf) *
            (double) STATFS_FRSIZE(statfsbuf);
        if (space < min_space) {
            return 0;
        }
    }
    jam = (double) STATFS_BAVAIL(statfsbuf) /
        (double) STATFS_BLOCKS(statfsbuf);
    if (jam >= maxdiskusagepct) {
        return 1;
    }
    return 0;
}