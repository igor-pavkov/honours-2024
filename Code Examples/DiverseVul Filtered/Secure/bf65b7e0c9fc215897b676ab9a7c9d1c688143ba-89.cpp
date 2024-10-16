int unit_setup_exec_runtime(Unit *u) {
        ExecRuntime **rt;
        size_t offset;
        Unit *other;
        Iterator i;
        void *v;
        int r;

        offset = UNIT_VTABLE(u)->exec_runtime_offset;
        assert(offset > 0);

        /* Check if there already is an ExecRuntime for this unit? */
        rt = (ExecRuntime**) ((uint8_t*) u + offset);
        if (*rt)
                return 0;

        /* Try to get it from somebody else */
        HASHMAP_FOREACH_KEY(v, other, u->dependencies[UNIT_JOINS_NAMESPACE_OF], i) {
                r = exec_runtime_acquire(u->manager, NULL, other->id, false, rt);
                if (r == 1)
                        return 1;
        }

        return exec_runtime_acquire(u->manager, unit_get_exec_context(u), u->id, true, rt);
}