static int assess_umask(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        char *copy = NULL;
        const char *d;
        uint64_t b;

        assert(ret_badness);
        assert(ret_description);

        if (!FLAGS_SET(info->_umask, 0002)) {
                d = "Files created by service are world-writable by default";
                b = 10;
        } else if (!FLAGS_SET(info->_umask, 0004)) {
                d = "Files created by service are world-readable by default";
                b = 5;
        } else if (!FLAGS_SET(info->_umask, 0020)) {
                d = "Files created by service are group-writable by default";
                b = 2;
        } else if (!FLAGS_SET(info->_umask, 0040)) {
                d = "Files created by service are group-readable by default";
                b = 1;
        } else {
                d = "Files created by service are accessible only by service's own user by default";
                b = 0;
        }

        copy = strdup(d);
        if (!copy)
                return log_oom();

        *ret_badness = b;
        *ret_description = copy;

        return 0;
}