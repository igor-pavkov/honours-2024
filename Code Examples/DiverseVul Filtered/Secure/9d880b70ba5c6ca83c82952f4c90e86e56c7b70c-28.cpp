static int assess_ip_address_allow(
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

        if (!info->ip_address_deny_all) {
                d = strdup("Service does not define an IP address whitelist");
                b = 10;
        } else if (info->ip_address_allow_other) {
                d = strdup("Service defines IP address whitelist with non-localhost entries");
                b = 5;
        } else if (info->ip_address_allow_localhost) {
                d = strdup("Service defines IP address whitelits with only localhost entries");
                b = 2;
        } else {
                d = strdup("Service blocks all IP address ranges");
                b = 0;
        }

        if (!d)
                return log_oom();

        *ret_badness = b;
        *ret_description = d;

        return 0;
}