static int assess_system_call_architectures(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        char *d;
        uint64_t b;

        assert(ret_badness);
        assert(ret_description);

        if (strv_isempty(info->system_call_architectures)) {
                b = 10;
                d = strdup("Service may execute system calls with all ABIs");
        } else if (strv_equal(info->system_call_architectures, STRV_MAKE("native"))) {
                b = 0;
                d = strdup("Service may execute system calls only with native ABI");
        } else {
                b = 8;
                d = strdup("Service may execute system calls with multiple ABIs");
        }

        if (!d)
                return log_oom();

        *ret_badness = b;
        *ret_description = d;

        return 0;
}