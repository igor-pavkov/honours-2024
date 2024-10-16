int manager_set_default_rlimits(Manager *m, struct rlimit **default_rlimit) {
        int i;

        assert(m);

        for (i = 0; i < _RLIMIT_MAX; i++) {
                m->rlimit[i] = mfree(m->rlimit[i]);

                if (!default_rlimit[i])
                        continue;

                m->rlimit[i] = newdup(struct rlimit, default_rlimit[i], 1);
                if (!m->rlimit[i])
                        return -ENOMEM;
        }

        return 0;
}