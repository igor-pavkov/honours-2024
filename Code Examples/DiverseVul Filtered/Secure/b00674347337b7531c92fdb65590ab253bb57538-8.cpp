int unit_name_path_escape(const char *f, char **ret) {
        _cleanup_free_ char *p = NULL;
        char *s;

        assert(f);
        assert(ret);

        p = strdup(f);
        if (!p)
                return -ENOMEM;

        path_simplify(p, false);

        if (empty_or_root(p))
                s = strdup("-");
        else {
                if (!path_is_normalized(p))
                        return -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        if (!s)
                return -ENOMEM;

        *ret = s;
        return 0;
}