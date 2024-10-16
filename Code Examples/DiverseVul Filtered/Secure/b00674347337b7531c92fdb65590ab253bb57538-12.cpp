int unit_name_replace_instance(const char *f, const char *i, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *p, *e;
        size_t a, b;

        assert(f);
        assert(i);
        assert(ret);

        if (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                return -EINVAL;
        if (!unit_instance_is_valid(i))
                return -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;
        b = strlen(i);

        s = new(char, a + 1 + b + strlen(e) + 1);
        if (!s)
                return -ENOMEM;

        strcpy(mempcpy(mempcpy(s, f, a + 1), i, b), e);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large */
        if (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                return -EINVAL;

        *ret = TAKE_PTR(s);
        return 0;
}