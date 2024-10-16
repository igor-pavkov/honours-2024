static void unit_tidy_watch_pids(Unit *u) {
        pid_t except1, except2;
        Iterator i;
        void *e;

        assert(u);

        /* Cleans dead PIDs from our list */

        except1 = unit_main_pid(u);
        except2 = unit_control_pid(u);

        SET_FOREACH(e, u->pids, i) {
                pid_t pid = PTR_TO_PID(e);

                if (pid == except1 || pid == except2)
                        continue;

                if (!pid_is_unwaited(pid))
                        unit_unwatch_pid(u, pid);
        }
}