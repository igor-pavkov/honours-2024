static int assess_ambient_capabilities(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        assert(ret_badness);
        assert(ret_description);

        *ret_badness = info->ambient_capabilities != 0;
        *ret_description = NULL;

        return 0;
}