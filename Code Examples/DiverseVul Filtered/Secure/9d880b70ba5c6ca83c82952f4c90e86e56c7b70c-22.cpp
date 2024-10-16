static int assess_protect_system(
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
        description = "Service has full access to the OS file hierarchy";

        r = parse_boolean(info->protect_system);
        if (r < 0) {
                if (streq_ptr(info->protect_system, "full")) {
                        badness = 3;
                        description = "Service has very limited write access to the OS file hierarchy";
                } else if (streq_ptr(info->protect_system, "strict")) {
                        badness = 0;
                        description = "Service has strict read-only access to the OS file hierarchy";
                }
        } else if (r > 0) {
                badness = 5;
                description = "Service has limited write access to the OS file hierarchy";
        }

        copy = strdup(description);
        if (!copy)
                return log_oom();

        *ret_badness = badness;
        *ret_description = copy;

        return 0;
}