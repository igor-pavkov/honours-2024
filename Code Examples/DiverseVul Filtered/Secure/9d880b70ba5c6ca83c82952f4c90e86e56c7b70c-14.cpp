static int assess_system_call_filter(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        const SyscallFilterSet *f;
        char *d = NULL;
        uint64_t b;

        assert(a);
        assert(info);
        assert(ret_badness);
        assert(ret_description);

        assert(a->parameter < _SYSCALL_FILTER_SET_MAX);
        f = syscall_filter_sets + a->parameter;

        if (!info->system_call_filter_whitelist && set_isempty(info->system_call_filter)) {
                d = strdup("Service does not filter system calls");
                b = 10;
        } else {
                bool bad;

                log_debug("Analyzing system call filter, checking against: %s", f->name);
                bad = syscall_names_in_filter(info->system_call_filter, info->system_call_filter_whitelist, f);
                log_debug("Result: %s", bad ? "bad" : "good");

                if (info->system_call_filter_whitelist) {
                        if (bad) {
                                (void) asprintf(&d, "System call whitelist defined for service, and %s is included", f->name);
                                b = 9;
                        } else {
                                (void) asprintf(&d, "System call whitelist defined for service, and %s is not included", f->name);
                                b = 0;
                        }
                } else {
                        if (bad) {
                                (void) asprintf(&d, "System call blacklist defined for service, and %s is not included", f->name);
                                b = 10;
                        } else {
                                (void) asprintf(&d, "System call blacklist defined for service, and %s is included", f->name);
                                b = 5;
                        }
                }
        }

        if (!d)
                return log_oom();

        *ret_badness = b;
        *ret_description = d;

        return 0;
}