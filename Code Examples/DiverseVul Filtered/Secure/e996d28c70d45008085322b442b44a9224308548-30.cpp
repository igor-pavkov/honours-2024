static void process_respfile(FILE * rfile, int pass)
{
    char *buffer, *p, *q, *prevarg;
    int bufsize, prevargsize;

    bufsize = prevargsize = ARG_BUF_DELTA;
    buffer = nasm_malloc(ARG_BUF_DELTA);
    prevarg = nasm_malloc(ARG_BUF_DELTA);
    prevarg[0] = '\0';

    while (1) {                 /* Loop to handle all lines in file */
        p = buffer;
        while (1) {             /* Loop to handle long lines */
            q = fgets(p, bufsize - (p - buffer), rfile);
            if (!q)
                break;
            p += strlen(p);
            if (p > buffer && p[-1] == '\n')
                break;
            if (p - buffer > bufsize - 10) {
                int offset;
                offset = p - buffer;
                bufsize += ARG_BUF_DELTA;
                buffer = nasm_realloc(buffer, bufsize);
                p = buffer + offset;
            }
        }

        if (!q && p == buffer) {
            if (prevarg[0])
                process_arg(prevarg, NULL, pass);
            nasm_free(buffer);
            nasm_free(prevarg);
            return;
        }

        /*
         * Play safe: remove CRs, LFs and any spurious ^Zs, if any of
         * them are present at the end of the line.
         */
        *(p = &buffer[strcspn(buffer, "\r\n\032")]) = '\0';

        while (p > buffer && nasm_isspace(p[-1]))
            *--p = '\0';

        p = nasm_skip_spaces(buffer);

        if (process_arg(prevarg, p, pass))
            *p = '\0';

        if ((int) strlen(p) > prevargsize - 10) {
            prevargsize += ARG_BUF_DELTA;
            prevarg = nasm_realloc(prevarg, prevargsize);
        }
        strncpy(prevarg, p, prevargsize);
    }
}