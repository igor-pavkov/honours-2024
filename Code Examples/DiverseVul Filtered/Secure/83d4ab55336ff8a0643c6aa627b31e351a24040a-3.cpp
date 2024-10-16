static bool validate_runtime_directory(pam_handle_t *handle, const char *path, uid_t uid) {
        struct stat st;

        assert(path);

        /* Just some extra paranoia: let's not set $XDG_RUNTIME_DIR if the directory we'd set it to isn't actually set
         * up properly for us. */

        if (lstat(path, &st) < 0) {
                pam_syslog(handle, LOG_ERR, "Failed to stat() runtime directory '%s': %s", path, strerror(errno));
                goto fail;
        }

        if (!S_ISDIR(st.st_mode)) {
                pam_syslog(handle, LOG_ERR, "Runtime directory '%s' is not actually a directory.", path);
                goto fail;
        }

        if (st.st_uid != uid) {
                pam_syslog(handle, LOG_ERR, "Runtime directory '%s' is not owned by UID " UID_FMT ", as it should.", path, uid);
                goto fail;
        }

        return true;

fail:
        pam_syslog(handle, LOG_WARNING, "Not setting $XDG_RUNTIME_DIR, as the directory is not in order.");
        return false;
}