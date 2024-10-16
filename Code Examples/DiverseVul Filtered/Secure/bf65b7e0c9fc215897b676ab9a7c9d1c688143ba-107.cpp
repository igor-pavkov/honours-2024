int unit_watch_pid(Unit *u, pid_t pid, bool exclusive) {
        int r;

        assert(u);
        assert(pid_is_valid(pid));

        /* Watch a specific PID */

        /* Caller might be sure that this PID belongs to this unit only. Let's take this
         * opportunity to remove any stalled references to this PID as they can be created
         * easily (when watching a process which is not our direct child). */
        if (exclusive)
                manager_unwatch_pid(u->manager, pid);

        r = set_ensure_allocated(&u->pids, NULL);
        if (r < 0)
                return r;

        r = hashmap_ensure_allocated(&u->manager->watch_pids, NULL);
        if (r < 0)
                return r;

        /* First try, let's add the unit keyed by "pid". */
        r = hashmap_put(u->manager->watch_pids, PID_TO_PTR(pid), u);
        if (r == -EEXIST)  {
                Unit **array;
                bool found = false;
                size_t n = 0;

                /* OK, the "pid" key is already assigned to a different unit. Let's see if the "-pid" key (which points
                 * to an array of Units rather than just a Unit), lists us already. */

                array = hashmap_get(u->manager->watch_pids, PID_TO_PTR(-pid));
                if (array)
                        for (; array[n]; n++)
                                if (array[n] == u)
                                        found = true;

                if (found) /* Found it already? if so, do nothing */
                        r = 0;
                else {
                        Unit **new_array;

                        /* Allocate a new array */
                        new_array = new(Unit*, n + 2);
                        if (!new_array)
                                return -ENOMEM;

                        memcpy_safe(new_array, array, sizeof(Unit*) * n);
                        new_array[n] = u;
                        new_array[n+1] = NULL;

                        /* Add or replace the old array */
                        r = hashmap_replace(u->manager->watch_pids, PID_TO_PTR(-pid), new_array);
                        if (r < 0) {
                                free(new_array);
                                return r;
                        }

                        free(array);
                }
        } else if (r < 0)
                return r;

        r = set_put(u->pids, PID_TO_PTR(pid));
        if (r < 0)
                return r;

        return 0;
}