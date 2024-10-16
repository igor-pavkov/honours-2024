static int assess_remove_ipc(
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
                *ret_badness = !info->remove_ipc;

        *ret_description = NULL;
        return 0;
}