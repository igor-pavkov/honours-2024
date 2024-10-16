static bool syscall_names_in_filter(Set *s, bool whitelist, const SyscallFilterSet *f) {
        const char *syscall;

        NULSTR_FOREACH(syscall, f->value) {
                int id;

                if (syscall[0] == '@') {
                        const SyscallFilterSet *g;

                        assert_se(g = syscall_filter_set_find(syscall));
                        if (syscall_names_in_filter(s, whitelist, g))
                                return true; /* bad! */

                        continue;
                }

                /* Let's see if the system call actually exists on this platform, before complaining */
                id = seccomp_syscall_resolve_name(syscall);
                if (id < 0)
                        continue;

                if (set_contains(s, syscall) == whitelist) {
                        log_debug("Offending syscall filter item: %s", syscall);
                        return true; /* bad! */
                }
        }

        return false;
}