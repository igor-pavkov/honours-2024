bool unit_shall_confirm_spawn(Unit *u) {
        assert(u);

        if (manager_is_confirm_spawn_disabled(u->manager))
                return false;

        /* For some reasons units remaining in the same process group
         * as PID 1 fail to acquire the console even if it's not used
         * by any process. So skip the confirmation question for them. */
        return !unit_get_exec_context(u)->same_pgrp;
}