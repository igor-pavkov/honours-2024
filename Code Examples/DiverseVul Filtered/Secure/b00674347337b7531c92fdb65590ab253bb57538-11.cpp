int unit_name_unescape(const char *f, char **ret) {
        _cleanup_free_ char *r = NULL;
        char *t;

        assert(f);

        r = strdup(f);
        if (!r)
                return -ENOMEM;

        for (t = r; *f; f++) {
                if (*f == '-')
                        *(t++) = '/';
                else if (*f == '\\') {
                        int a, b;

                        if (f[1] != 'x')
                                return -EINVAL;

                        a = unhexchar(f[2]);
                        if (a < 0)
                                return -EINVAL;

                        b = unhexchar(f[3]);
                        if (b < 0)
                                return -EINVAL;

                        *(t++) = (char) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } else
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        return 0;
}