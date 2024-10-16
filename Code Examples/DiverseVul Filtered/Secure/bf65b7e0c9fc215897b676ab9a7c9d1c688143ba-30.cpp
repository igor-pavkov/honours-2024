char* unit_concat_strv(char **l, UnitWriteFlags flags) {
        _cleanup_free_ char *result = NULL;
        size_t n = 0, allocated = 0;
        char **i;

        /* Takes a list of strings, escapes them, and concatenates them. This may be used to format command lines in a
         * way suitable for ExecStart= stanzas */

        STRV_FOREACH(i, l) {
                _cleanup_free_ char *buf = NULL;
                const char *p;
                size_t a;
                char *q;

                p = unit_escape_setting(*i, flags, &buf);
                if (!p)
                        return NULL;

                a = (n > 0) + 1 + strlen(p) + 1; /* separating space + " + entry + " */
                if (!GREEDY_REALLOC(result, allocated, n + a + 1))
                        return NULL;

                q = result + n;
                if (n > 0)
                        *(q++) = ' ';

                *(q++) = '"';
                q = stpcpy(q, p);
                *(q++) = '"';

                n += a;
        }

        if (!GREEDY_REALLOC(result, allocated, n + 1))
                return NULL;

        result[n] = 0;

        return TAKE_PTR(result);
}