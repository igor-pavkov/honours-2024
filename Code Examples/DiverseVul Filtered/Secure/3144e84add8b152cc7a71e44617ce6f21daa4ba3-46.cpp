static char *check_tasm_directive(char *line)
{
    int32_t i, j, k, m, len;
    char *p, *q, *oldline, oldchar;

    p = nasm_skip_spaces(line);

    /* Binary search for the directive name */
    i = -1;
    j = ARRAY_SIZE(tasm_directives);
    q = nasm_skip_word(p);
    len = q - p;
    if (len) {
        oldchar = p[len];
        p[len] = 0;
        while (j - i > 1) {
            k = (j + i) / 2;
            m = nasm_stricmp(p, tasm_directives[k]);
            if (m == 0) {
                /* We have found a directive, so jam a % in front of it
                 * so that NASM will then recognise it as one if it's own.
                 */
                p[len] = oldchar;
                len = strlen(p);
                oldline = line;
                line = nasm_malloc(len + 2);
                line[0] = '%';
                if (k == TM_IFDIFI) {
                    /*
                     * NASM does not recognise IFDIFI, so we convert
                     * it to %if 0. This is not used in NASM
                     * compatible code, but does need to parse for the
                     * TASM macro package.
                     */
                    strcpy(line + 1, "if 0");
                } else {
                    memcpy(line + 1, p, len + 1);
                }
                nasm_free(oldline);
                return line;
            } else if (m < 0) {
                j = k;
            } else
                i = k;
        }
        p[len] = oldchar;
    }
    return line;
}