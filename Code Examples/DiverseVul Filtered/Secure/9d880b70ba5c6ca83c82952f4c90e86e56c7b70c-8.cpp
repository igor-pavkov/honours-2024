static int assess_root_directory(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        assert(ret_badness);
        assert(ret_description);

        *ret_badness =
                (isempty(info->root_directory) ||
                 path_equal(info->root_directory, "/")) &&
                (isempty(info->root_image) ||
                 path_equal(info->root_image, "/"));
        *ret_description = NULL;

        return 0;
}