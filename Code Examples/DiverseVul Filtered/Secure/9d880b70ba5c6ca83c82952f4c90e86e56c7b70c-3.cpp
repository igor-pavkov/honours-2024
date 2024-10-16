static int assess_device_allow(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        char *d = NULL;
        uint64_t b;

        assert(info);
        assert(ret_badness);
        assert(ret_description);

        if (STRPTR_IN_SET(info->device_policy, "strict", "closed")) {

                if (info->device_allow_non_empty) {
                        d = strdup("Service has a device ACL with some special devices");
                        b = 5;
                } else {
                        d = strdup("Service has a minimal device ACL");
                        b = 0;
                }
        } else {
                d = strdup("Service has no device ACL");
                b = 10;
        }

        if (!d)
                return log_oom();

        *ret_badness = b;
        *ret_description = d;

        return 0;
}