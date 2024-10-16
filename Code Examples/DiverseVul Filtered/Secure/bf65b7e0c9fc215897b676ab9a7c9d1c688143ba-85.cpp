static void unit_remove_transient(Unit *u) {
        char **i;

        assert(u);

        if (!u->transient)
                return;

        if (u->fragment_path)
                (void) unlink(u->fragment_path);

        STRV_FOREACH(i, u->dropin_paths) {
                _cleanup_free_ char *p = NULL, *pp = NULL;

                p = dirname_malloc(*i); /* Get the drop-in directory from the drop-in file */
                if (!p)
                        continue;

                pp = dirname_malloc(p); /* Get the config directory from the drop-in directory */
                if (!pp)
                        continue;

                /* Only drop transient drop-ins */
                if (!path_equal(u->manager->lookup_paths.transient, pp))
                        continue;

                (void) unlink(*i);
                (void) rmdir(p);
        }
}