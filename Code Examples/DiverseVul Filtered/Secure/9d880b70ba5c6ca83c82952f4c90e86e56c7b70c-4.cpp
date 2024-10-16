static int assess_supplementary_groups(
                const struct security_assessor *a,
                const struct security_info *info,
                const void *data,
                uint64_t *ret_badness,
                char **ret_description) {

        assert(ret_badness);
        assert(ret_description);

        if (security_info_runs_privileged(info))
                *ret_badness = UINT64_MAX;
        else
                *ret_badness = !strv_isempty(info->supplementary_groups);

        *ret_description = NULL;
        return 0;
}