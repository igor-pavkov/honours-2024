static int assess_capability_bounding_set(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        assert(ret_badness);
        assert(ret_description);

        *ret_badness = !!(info->capability_bounding_set & a->parameter);
        *ret_description = NULL;

        return 0;
}