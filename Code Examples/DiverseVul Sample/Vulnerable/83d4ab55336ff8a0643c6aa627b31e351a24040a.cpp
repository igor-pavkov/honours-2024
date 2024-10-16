static const char* getenv_harder(pam_handle_t *handle, const char *key, const char *fallback) {
        const char *v;

        assert(handle);
        assert(key);

        /* Looks for an environment variable, preferrably in the environment block associated with the specified PAM
         * handle, falling back to the process' block instead. */

        v = pam_getenv(handle, key);
        if (!isempty(v))
                return v;

        v = getenv(key);
        if (!isempty(v))
                return v;

        return fallback;
}