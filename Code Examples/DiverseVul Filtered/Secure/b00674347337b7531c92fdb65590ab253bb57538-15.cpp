int unit_name_to_instance(const char *n, char **ret) {
        const char *p, *d;

        assert(n);

        if (!unit_name_is_valid(n, UNIT_NAME_ANY))
                return -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        if (!p) {
                if (ret)
                        *ret = NULL;
                return UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        if (!d)
                return -EINVAL;

        if (ret) {
                char *i = strndup(p, d-p);
                if (!i)
                        return -ENOMEM;

                *ret = i;
        }
        return d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}