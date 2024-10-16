bool unit_need_daemon_reload(Unit *u) {
        _cleanup_strv_free_ char **t = NULL;
        char **path;

        assert(u);

        /* For unit files, we allow masking… */
        if (fragment_mtime_newer(u->fragment_path, u->fragment_mtime,
                                 u->load_state == UNIT_MASKED))
                return true;

        /* Source paths should not be masked… */
        if (fragment_mtime_newer(u->source_path, u->source_mtime, false))
                return true;

        if (u->load_state == UNIT_LOADED)
                (void) unit_find_dropin_paths(u, &t);
        if (!strv_equal(u->dropin_paths, t))
                return true;

        /* … any drop-ins that are masked are simply omitted from the list. */
        STRV_FOREACH(path, u->dropin_paths)
                if (fragment_mtime_newer(*path, u->dropin_mtime, false))
                        return true;

        return false;
}