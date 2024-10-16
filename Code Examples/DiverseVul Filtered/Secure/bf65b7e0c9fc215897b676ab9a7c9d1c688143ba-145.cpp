static bool fragment_mtime_newer(const char *path, usec_t mtime, bool path_masked) {
        struct stat st;

        if (!path)
                return false;

        /* If the source is some virtual kernel file system, then we assume we watch it anyway, and hence pretend we
         * are never out-of-date. */
        if (PATH_STARTSWITH_SET(path, "/proc", "/sys"))
                return false;

        if (stat(path, &st) < 0)
                /* What, cannot access this anymore? */
                return true;

        if (path_masked)
                /* For masked files check if they are still so */
                return !null_or_empty(&st);
        else
                /* For non-empty files check the mtime */
                return timespec_load(&st.st_mtim) > mtime;

        return false;
}