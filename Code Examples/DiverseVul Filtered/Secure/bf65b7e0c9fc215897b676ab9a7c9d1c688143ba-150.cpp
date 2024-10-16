int unit_write_settingf(Unit *u, UnitWriteFlags flags, const char *name, const char *format, ...) {
        _cleanup_free_ char *p = NULL;
        va_list ap;
        int r;

        assert(u);
        assert(name);
        assert(format);

        if (UNIT_WRITE_FLAGS_NOOP(flags))
                return 0;

        va_start(ap, format);
        r = vasprintf(&p, format, ap);
        va_end(ap);

        if (r < 0)
                return -ENOMEM;

        return unit_write_setting(u, flags, name, p);
}