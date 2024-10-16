void doallo(const off_t size)
{
    int ret = -1;
#ifdef QUOTAS
    Quota quota;
#endif

    if (size <= 0) {
        ret = 0;
    } else if (ul_check_free_space(wd, (double) size) != 0) {
        ret = 0;
    }
#ifdef QUOTAS
    if (quota_update(&quota, 0LL, 0LL, NULL) == 0) {
        if (quota.files >= user_quota_files ||
            quota.size >= user_quota_size ||
            (unsigned long long) size > user_quota_size - quota.size) {
            ret = -1;
        }
    }
#endif
    if (ret == 0) {
#ifdef DISABLE_HUMOR
        addreply_noformat(200, "OK");
#else
        addreply_noformat(200, "A L'HUILE");
#endif
    } else {
        addreply_noformat(552, MSG_NO_DISK_SPACE);
    }
}