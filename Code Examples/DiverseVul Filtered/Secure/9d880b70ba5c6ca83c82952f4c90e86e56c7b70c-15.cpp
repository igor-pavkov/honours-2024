static int assess_protect_home(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        const char *description;
        uint64_t badness;
        char *copy;
        int r;

        assert(ret_badness);
        assert(ret_description);

        badness = 10;
        description = "Service has full access to home directories";

        r = parse_boolean(info->protect_home);
        if (r < 0) {
                if (streq_ptr(info->protect_home, "read-only")) {
                        badness = 5;
                        description = "Service has read-only access to home directories";
                } else if (streq_ptr(info->protect_home, "tmpfs")) {
                        badness = 1;
                        description = "Service has access to fake empty home directories";
                }
        } else if (r > 0) {
                badness = 0;
                description = "Service has no access to home directories";
        }

        copy = strdup(description);
        if (!copy)
                return log_oom();

        *ret_badness = badness;
        *ret_description = copy;

        return 0;
}