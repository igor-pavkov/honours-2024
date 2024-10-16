static int manager_default_environment(Manager *m) {
        assert(m);

        if (MANAGER_IS_SYSTEM(m)) {
                /* The system manager always starts with a clean
                 * environment for its children. It does not import
                 * the kernel or the parents exported variables.
                 *
                 * The initial passed environ is untouched to keep
                 * /proc/self/environ valid; it is used for tagging
                 * the init process inside containers. */
                m->environment = strv_new("PATH=" DEFAULT_PATH,
                                          NULL);

                /* Import locale variables LC_*= from configuration */
                locale_setup(&m->environment);
        } else {
                /* The user manager passes its own environment
                 * along to its children. */
                m->environment = strv_copy(environ);
        }

        if (!m->environment)
                return -ENOMEM;

        manager_clean_environment(m);
        strv_sort(m->environment);

        return 0;
}