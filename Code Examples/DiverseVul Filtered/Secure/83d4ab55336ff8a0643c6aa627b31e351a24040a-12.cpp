static int update_environment(pam_handle_t *handle, const char *key, const char *value) {
        int r;

        assert(handle);
        assert(key);

        /* Updates the environment, but only if there's actually a value set. Also, log about errors */

        if (isempty(value))
                return PAM_SUCCESS;

        r = pam_misc_setenv(handle, key, value, 0);
        if (r != PAM_SUCCESS)
                pam_syslog(handle, LOG_ERR, "Failed to set environment variable %s.", key);

        return r;
}