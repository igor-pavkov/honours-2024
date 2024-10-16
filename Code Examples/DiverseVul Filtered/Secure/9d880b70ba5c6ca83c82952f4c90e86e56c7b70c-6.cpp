static int assess_notify_access(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        assert(ret_badness);
        assert(ret_description);

        *ret_badness = streq_ptr(info->notify_access, "all");
        *ret_description = NULL;

        return 0;
}