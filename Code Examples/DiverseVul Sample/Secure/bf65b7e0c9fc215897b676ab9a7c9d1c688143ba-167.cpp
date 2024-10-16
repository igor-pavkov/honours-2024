static int unit_deserialize_job(Unit *u, FILE *f) {
        _cleanup_(job_freep) Job *j = NULL;
        int r;

        assert(u);
        assert(f);

        j = job_new_raw(u);
        if (!j)
                return log_oom();

        r = job_deserialize(j, f);
        if (r < 0)
                return r;

        r = job_install_deserialized(j);
        if (r < 0)
                return r;

        TAKE_PTR(j);
        return 0;
}