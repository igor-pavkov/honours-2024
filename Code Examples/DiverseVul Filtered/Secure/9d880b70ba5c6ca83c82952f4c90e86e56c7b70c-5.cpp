static int assess_restrict_namespaces(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        assert(ret_badness);
        assert(ret_description);

        *ret_badness = !!(info->restrict_namespaces & a->parameter);
        *ret_description = NULL;

        return 0;
}