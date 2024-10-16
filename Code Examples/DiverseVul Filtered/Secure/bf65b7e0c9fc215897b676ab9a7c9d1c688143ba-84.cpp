int unit_set_exec_params(Unit *u, ExecParameters *p) {
        int r;

        assert(u);
        assert(p);

        /* Copy parameters from manager */
        r = manager_get_effective_environment(u->manager, &p->environment);
        if (r < 0)
                return r;

        p->confirm_spawn = manager_get_confirm_spawn(u->manager);
        p->cgroup_supported = u->manager->cgroup_supported;
        p->prefix = u->manager->prefix;
        SET_FLAG(p->flags, EXEC_PASS_LOG_UNIT|EXEC_CHOWN_DIRECTORIES, MANAGER_IS_SYSTEM(u->manager));

        /* Copy paramaters from unit */
        p->cgroup_path = u->cgroup_path;
        SET_FLAG(p->flags, EXEC_CGROUP_DELEGATE, unit_cgroup_delegate(u));

        return 0;
}