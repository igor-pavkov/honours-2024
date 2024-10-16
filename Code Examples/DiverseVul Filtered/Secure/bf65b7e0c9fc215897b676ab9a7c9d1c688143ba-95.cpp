int unit_new_for_name(Manager *m, size_t size, const char *name, Unit **ret) {
        _cleanup_(unit_freep) Unit *u = NULL;
        int r;

        u = unit_new(m, size);
        if (!u)
                return -ENOMEM;

        r = unit_add_name(u, name);
        if (r < 0)
                return r;

        *ret = TAKE_PTR(u);

        return r;
}