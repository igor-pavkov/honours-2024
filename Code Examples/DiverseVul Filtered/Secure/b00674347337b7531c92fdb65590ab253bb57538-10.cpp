int unit_name_change_suffix(const char *n, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        size_t a, b;
        char *e;

        assert(n);
        assert(suffix);
        assert(ret);

        if (!unit_name_is_valid(n, UNIT_NAME_ANY))
                return -EINVAL;

        if (!unit_suffix_is_valid(suffix))
                return -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(char, a + b + 1);
        if (!s)
                return -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        if (!unit_name_is_valid(s, UNIT_NAME_ANY))
                return -EINVAL;

        *ret = TAKE_PTR(s);
        return 0;
}