int unit_make_transient(Unit *u) {
        _cleanup_free_ char *path = NULL;
        FILE *f;

        assert(u);

        if (!UNIT_VTABLE(u)->can_transient)
                return -EOPNOTSUPP;

        (void) mkdir_p_label(u->manager->lookup_paths.transient, 0755);

        path = strjoin(u->manager->lookup_paths.transient, "/", u->id);
        if (!path)
                return -ENOMEM;

        /* Let's open the file we'll write the transient settings into. This file is kept open as long as we are
         * creating the transient, and is closed in unit_load(), as soon as we start loading the file. */

        RUN_WITH_UMASK(0022) {
                f = fopen(path, "we");
                if (!f)
                        return -errno;
        }

        safe_fclose(u->transient_file);
        u->transient_file = f;

        free_and_replace(u->fragment_path, path);

        u->source_path = mfree(u->source_path);
        u->dropin_paths = strv_free(u->dropin_paths);
        u->fragment_mtime = u->source_mtime = u->dropin_mtime = 0;

        u->load_state = UNIT_STUB;
        u->load_error = 0;
        u->transient = true;

        unit_add_to_dbus_queue(u);
        unit_add_to_gc_queue(u);

        fputs("# This is a transient unit file, created programmatically via the systemd API. Do not edit.\n",
              u->transient_file);

        return 0;
}