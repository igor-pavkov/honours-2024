int unit_name_from_path(const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        int r;

        assert(path);
        assert(suffix);
        assert(ret);

        if (!unit_suffix_is_valid(suffix))
                return -EINVAL;

        r = unit_name_path_escape(path, &p);
        if (r < 0)
                return r;

        s = strjoin(p, suffix);
        if (!s)
                return -ENOMEM;

        if (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                return -ENAMETOOLONG;

        /* Refuse this if this got too long or for some other reason didn't result in a valid name */
        if (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                return -EINVAL;

        *ret = TAKE_PTR(s);
        return 0;
}