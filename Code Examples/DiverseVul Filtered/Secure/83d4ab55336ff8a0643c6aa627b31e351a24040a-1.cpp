static int get_user_data(
                pam_handle_t *handle,
                const char **ret_username,
                struct passwd **ret_pw) {

        const char *username = NULL;
        struct passwd *pw = NULL;
        int r;

        assert(handle);
        assert(ret_username);
        assert(ret_pw);

        r = pam_get_user(handle, &username, NULL);
        if (r != PAM_SUCCESS) {
                pam_syslog(handle, LOG_ERR, "Failed to get user name.");
                return r;
        }

        if (isempty(username)) {
                pam_syslog(handle, LOG_ERR, "User name not valid.");
                return PAM_AUTH_ERR;
        }

        pw = pam_modutil_getpwnam(handle, username);
        if (!pw) {
                pam_syslog(handle, LOG_ERR, "Failed to get user data.");
                return PAM_USER_UNKNOWN;
        }

        *ret_pw = pw;
        *ret_username = username;

        return PAM_SUCCESS;
}