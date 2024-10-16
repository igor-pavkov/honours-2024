static int assess_user(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        _cleanup_free_ char *d = NULL;
        uint64_t b;

        assert(ret_badness);
        assert(ret_description);

        if (streq_ptr(info->user, NOBODY_USER_NAME)) {
                d = strdup("Service runs under as '" NOBODY_USER_NAME "' user, which should not be used for services");
                b = 9;
        } else if (info->dynamic_user && !STR_IN_SET(info->user, "0", "root")) {
                d = strdup("Service runs under a transient non-root user identity");
                b = 0;
        } else if (info->user && !STR_IN_SET(info->user, "0", "root", "")) {
                d = strdup("Service runs under a static non-root user identity");
                b = 0;
        } else {
                *ret_badness = 10;
                *ret_description = NULL;
                return 0;
        }

        if (!d)
                return log_oom();

        *ret_badness = b;
        *ret_description = TAKE_PTR(d);

        return 0;
}