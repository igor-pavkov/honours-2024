static void unit_init(Unit *u) {
        CGroupContext *cc;
        ExecContext *ec;
        KillContext *kc;

        assert(u);
        assert(u->manager);
        assert(u->type >= 0);

        cc = unit_get_cgroup_context(u);
        if (cc) {
                cgroup_context_init(cc);

                /* Copy in the manager defaults into the cgroup
                 * context, _before_ the rest of the settings have
                 * been initialized */

                cc->cpu_accounting = u->manager->default_cpu_accounting;
                cc->io_accounting = u->manager->default_io_accounting;
                cc->ip_accounting = u->manager->default_ip_accounting;
                cc->blockio_accounting = u->manager->default_blockio_accounting;
                cc->memory_accounting = u->manager->default_memory_accounting;
                cc->tasks_accounting = u->manager->default_tasks_accounting;
                cc->ip_accounting = u->manager->default_ip_accounting;

                if (u->type != UNIT_SLICE)
                        cc->tasks_max = u->manager->default_tasks_max;
        }

        ec = unit_get_exec_context(u);
        if (ec) {
                exec_context_init(ec);

                ec->keyring_mode = MANAGER_IS_SYSTEM(u->manager) ?
                        EXEC_KEYRING_SHARED : EXEC_KEYRING_INHERIT;
        }

        kc = unit_get_kill_context(u);
        if (kc)
                kill_context_init(kc);

        if (UNIT_VTABLE(u)->init)
                UNIT_VTABLE(u)->init(u);
}