int manager_get_job_from_dbus_path(Manager *m, const char *s, Job **_j) {
        const char *p;
        unsigned id;
        Job *j;
        int r;

        assert(m);
        assert(s);
        assert(_j);

        p = startswith(s, "/org/freedesktop/systemd1/job/");
        if (!p)
                return -EINVAL;

        r = safe_atou(p, &id);
        if (r < 0)
                return r;

        j = manager_get_job(m, id);
        if (!j)
                return -ENOENT;

        *_j = j;

        return 0;
}