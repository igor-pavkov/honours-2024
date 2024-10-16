int unit_exit_status(Unit *u) {
        assert(u);

        /* Returns the exit status to propagate for the most recent cycle of this unit. Returns a value in the range
         * 0…255 if there's something to propagate. EOPNOTSUPP if the concept does not apply to this unit type, ENODATA
         * if no data is currently known (for example because the unit hasn't deactivated yet) and EBADE if the main
         * service process has exited abnormally (signal/coredump). */

        if (!UNIT_VTABLE(u)->exit_status)
                return -EOPNOTSUPP;

        return UNIT_VTABLE(u)->exit_status(u);
}