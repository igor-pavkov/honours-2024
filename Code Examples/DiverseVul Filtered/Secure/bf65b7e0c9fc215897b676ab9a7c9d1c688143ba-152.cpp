void unit_unwatch_pid(Unit *u, pid_t pid) {
        Unit **array;

        assert(u);
        assert(pid_is_valid(pid));

        /* First let's drop the unit in case it's keyed as "pid". */
        (void) hashmap_remove_value(u->manager->watch_pids, PID_TO_PTR(pid), u);

        /* Then, let's also drop the unit, in case it's in the array keyed by -pid */
        array = hashmap_get(u->manager->watch_pids, PID_TO_PTR(-pid));
        if (array) {
                size_t n, m = 0;

                /* Let's iterate through the array, dropping our own entry */
                for (n = 0; array[n]; n++)
                        if (array[n] != u)
                                array[m++] = array[n];
                array[m] = NULL;

                if (m == 0) {
                        /* The array is now empty, remove the entire entry */
                        assert(hashmap_remove(u->manager->watch_pids, PID_TO_PTR(-pid)) == array);
                        free(array);
                }
        }

        (void) set_remove(u->pids, PID_TO_PTR(pid));
}