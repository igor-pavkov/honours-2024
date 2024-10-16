static int assess_bool(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        const bool *b = data;

        assert(b);
        assert(ret_badness);
        assert(ret_description);

        *ret_badness = a->parameter ? *b : !*b;
        *ret_description = NULL;

        return 0;
}