static void manager_build_unit_path_cache(Manager *m) {
        char **i;
        int r;

        assert(m);

        set_free_free(m->unit_path_cache);

        m->unit_path_cache = set_new(&string_hash_ops);
        if (!m->unit_path_cache) {
                r = -ENOMEM;
                goto fail;
        }

        /* This simply builds a list of files we know exist, so that
         * we don't always have to go to disk */

        STRV_FOREACH(i, m->lookup_paths.search_path) {
                _cleanup_closedir_ DIR *d = NULL;
                struct dirent *de;

                d = opendir(*i);
                if (!d) {
                        if (errno != ENOENT)
                                log_warning_errno(errno, "Failed to open directory %s, ignoring: %m", *i);
                        continue;
                }

                FOREACH_DIRENT(de, d, r = -errno; goto fail) {
                        char *p;

                        p = strjoin(streq(*i, "/") ? "" : *i, "/", de->d_name, NULL);
                        if (!p) {
                                r = -ENOMEM;
                                goto fail;
                        }

                        r = set_consume(m->unit_path_cache, p);
                        if (r < 0)
                                goto fail;
                }
        }

        return;

fail:
        log_warning_errno(r, "Failed to build unit path cache, proceeding without: %m");
        m->unit_path_cache = set_free_free(m->unit_path_cache);
}