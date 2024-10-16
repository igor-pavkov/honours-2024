int unit_write_setting(Unit *u, UnitWriteFlags flags, const char *name, const char *data) {
        _cleanup_free_ char *p = NULL, *q = NULL, *escaped = NULL;
        const char *dir, *wrapped;
        int r;

        assert(u);
        assert(name);
        assert(data);

        if (UNIT_WRITE_FLAGS_NOOP(flags))
                return 0;

        data = unit_escape_setting(data, flags, &escaped);
        if (!data)
                return -ENOMEM;

        /* Prefix the section header. If we are writing this out as transient file, then let's suppress this if the
         * previous section header is the same */

        if (flags & UNIT_PRIVATE) {
                if (!UNIT_VTABLE(u)->private_section)
                        return -EINVAL;

                if (!u->transient_file || u->last_section_private < 0)
                        data = strjoina("[", UNIT_VTABLE(u)->private_section, "]\n", data);
                else if (u->last_section_private == 0)
                        data = strjoina("\n[", UNIT_VTABLE(u)->private_section, "]\n", data);
        } else {
                if (!u->transient_file || u->last_section_private < 0)
                        data = strjoina("[Unit]\n", data);
                else if (u->last_section_private > 0)
                        data = strjoina("\n[Unit]\n", data);
        }

        if (u->transient_file) {
                /* When this is a transient unit file in creation, then let's not create a new drop-in but instead
                 * write to the transient unit file. */
                fputs(data, u->transient_file);

                if (!endswith(data, "\n"))
                        fputc('\n', u->transient_file);

                /* Remember which section we wrote this entry to */
                u->last_section_private = !!(flags & UNIT_PRIVATE);
                return 0;
        }

        dir = unit_drop_in_dir(u, flags);
        if (!dir)
                return -EINVAL;

        wrapped = strjoina("# This is a drop-in unit file extension, created via \"systemctl set-property\"\n"
                           "# or an equivalent operation. Do not edit.\n",
                           data,
                           "\n");

        r = drop_in_file(dir, u->id, 50, name, &p, &q);
        if (r < 0)
                return r;

        (void) mkdir_p_label(p, 0755);
        r = write_string_file_atomic_label(q, wrapped);
        if (r < 0)
                return r;

        r = strv_push(&u->dropin_paths, q);
        if (r < 0)
                return r;
        q = NULL;

        strv_uniq(u->dropin_paths);

        u->dropin_mtime = now(CLOCK_REALTIME);

        return 0;
}