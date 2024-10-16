int unit_success_action_exit_status(Unit *u) {
        int r;

        assert(u);

        /* Returns the exit status to propagate on success, or an error if there's nothing to propagate */

        if (u->success_action_exit_status >= 0)
                return u->success_action_exit_status;

        r = unit_exit_status(u);
        if (r == -EBADE) /* Exited, but not cleanly (i.e. by signal or such) */
                return 255;

        return r;
}