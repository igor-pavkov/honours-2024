static AuthResult pw_check(const char *account, const char *password,
                           const struct sockaddr_storage * const sa,
                           const struct sockaddr_storage * const peer)
{
    Authentications *auth_scan = first_authentications;
    AuthResult result;

    result.auth_ok = -1;
    while (auth_scan != NULL) {
#ifdef THROTTLING
        result.throttling_bandwidth_ul = throttling_bandwidth_ul;
        result.throttling_bandwidth_dl = throttling_bandwidth_dl;
        result.throttling_ul_changed = result.throttling_dl_changed = 0;
#endif
#ifdef QUOTAS
        result.user_quota_size = user_quota_size;
        result.user_quota_files = user_quota_files;
        result.quota_size_changed = result.quota_files_changed = 0;
#endif
#ifdef RATIOS
        result.ratio_upload = ratio_upload;
        result.ratio_download = ratio_download;
        result.ratio_ul_changed = result.ratio_dl_changed = 0;
#endif
#ifdef PER_USER_LIMITS
        result.per_user_max = per_user_max;
#endif
        result.backend_data = NULL;
        auth_scan->auth->check(&result, account, password, sa, peer);
        if (result.auth_ok < 0) {
            break;
        } else if (result.auth_ok > 0) {
#ifdef THROTTLING
            if ((result.throttling_ul_changed |
                 result.throttling_dl_changed) != 0) {
                if (result.throttling_ul_changed != 0 &&
                    result.throttling_bandwidth_ul > 0UL) {
                    throttling_bandwidth_ul = result.throttling_bandwidth_ul;
                }
                if (result.throttling_dl_changed != 0 &&
                    result.throttling_bandwidth_dl > 0UL) {
                    throttling_bandwidth_dl = result.throttling_bandwidth_dl;
                }
                throttling_delay = 1000000 /
                    (throttling_bandwidth_dl | throttling_bandwidth_ul);
                throttling = 2;
            }
#endif
#ifdef QUOTAS
            if (result.quota_size_changed != 0) {
                user_quota_size = result.user_quota_size;
            }
            if (result.quota_files_changed != 0) {
                user_quota_files = result.user_quota_files;
            }
#endif
#ifdef RATIOS
            if (result.ratio_ul_changed != 0) {
                ratio_upload = result.ratio_upload;
                ratio_for_non_anon = 1;
            }
            if (result.ratio_dl_changed != 0) {
                ratio_download = result.ratio_download;
            }
#endif
#ifdef PER_USER_LIMITS
            per_user_max = result.per_user_max;
#endif

#ifdef NON_ROOT_FTP
            result.uid = geteuid();
            result.gid = getegid();
#endif

            return result;
        }
        auth_scan = auth_scan->next;
    }

    return result;
}