static int assess_keyring_mode(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        assert(ret_badness);
        assert(ret_description);

        *ret_badness = !streq_ptr(info->keyring_mode, "private");
        *ret_description = NULL;

        return 0;
}