static int user_from_unit_name(Unit *u, char **ret) {

        static const uint8_t hash_key[] = {
                0x58, 0x1a, 0xaf, 0xe6, 0x28, 0x58, 0x4e, 0x96,
                0xb4, 0x4e, 0xf5, 0x3b, 0x8c, 0x92, 0x07, 0xec
        };

        _cleanup_free_ char *n = NULL;
        int r;

        r = unit_name_to_prefix(u->id, &n);
        if (r < 0)
                return r;

        if (valid_user_group_name(n)) {
                *ret = TAKE_PTR(n);
                return 0;
        }

        /* If we can't use the unit name as a user name, then let's hash it and use that */
        if (asprintf(ret, "_du%016" PRIx64, siphash24(n, strlen(n), hash_key)) < 0)
                return -ENOMEM;

        return 0;
}