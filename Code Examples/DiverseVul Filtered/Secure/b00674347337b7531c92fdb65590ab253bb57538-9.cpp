int unit_name_path_unescape(const char *f, char **ret) {
        _cleanup_free_ char *s = NULL;
        int r;

        assert(f);

        if (isempty(f))
                return -EINVAL;

        if (streq(f, "-")) {
                s = strdup("/");
                if (!s)
                        return -ENOMEM;
        } else {
                _cleanup_free_ char *w = NULL;

                r = unit_name_unescape(f, &w);
                if (r < 0)
                        return r;

                /* Don't accept trailing or leading slashes */
                if (startswith(w, "/") || endswith(w, "/"))
                        return -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                if (!s)
                        return -ENOMEM;

                if (!path_is_normalized(s))
                        return -EINVAL;
        }

        if (ret)
                *ret = TAKE_PTR(s);

        return 0;
}