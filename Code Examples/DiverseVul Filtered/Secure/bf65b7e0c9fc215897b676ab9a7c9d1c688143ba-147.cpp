int unit_deserialize_skip(FILE *f) {
        int r;
        assert(f);

        /* Skip serialized data for this unit. We don't know what it is. */

        for (;;) {
                _cleanup_free_ char *line = NULL;
                char *l;

                r = read_line(f, LONG_LINE_MAX, &line);
                if (r < 0)
                        return log_error_errno(r, "Failed to read serialization line: %m");
                if (r == 0)
                        return 0;

                l = strstrip(line);

                /* End marker */
                if (isempty(l))
                        return 1;
        }
}