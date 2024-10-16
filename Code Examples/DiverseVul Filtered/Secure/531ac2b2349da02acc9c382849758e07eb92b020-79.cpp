int manager_environment_add(Manager *m, char **minus, char **plus) {
        char **a = NULL, **b = NULL, **l;
        assert(m);

        l = m->environment;

        if (!strv_isempty(minus)) {
                a = strv_env_delete(l, 1, minus);
                if (!a)
                        return -ENOMEM;

                l = a;
        }

        if (!strv_isempty(plus)) {
                b = strv_env_merge(2, l, plus);
                if (!b) {
                        strv_free(a);
                        return -ENOMEM;
                }

                l = b;
        }

        if (m->environment != l)
                strv_free(m->environment);
        if (a != l)
                strv_free(a);
        if (b != l)
                strv_free(b);

        m->environment = l;
        manager_clean_environment(m);
        strv_sort(m->environment);

        return 0;
}