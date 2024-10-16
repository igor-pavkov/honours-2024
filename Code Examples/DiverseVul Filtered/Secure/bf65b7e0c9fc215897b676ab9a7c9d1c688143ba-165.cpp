const char *unit_label_path(Unit *u) {
        const char *p;

        /* Returns the file system path to use for MAC access decisions, i.e. the file to read the SELinux label off
         * when validating access checks. */

        p = u->source_path ?: u->fragment_path;
        if (!p)
                return NULL;

        /* If a unit is masked, then don't read the SELinux label of /dev/null, as that really makes no sense */
        if (path_equal(p, "/dev/null"))
                return NULL;

        return p;
}