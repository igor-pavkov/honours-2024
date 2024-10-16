static char *prepreproc(char *line)
{
    int lineno, fnlen;
    char *fname, *oldline;

    if (line[0] == '#' && line[1] == ' ') {
        oldline = line;
        fname = oldline + 2;
        lineno = atoi(fname);
        fname += strspn(fname, "0123456789 ");
        if (*fname == '"')
            fname++;
        fnlen = strcspn(fname, "\"");
        line = nasm_malloc(20 + fnlen);
        snprintf(line, 20 + fnlen, "%%line %d %.*s", lineno, fnlen, fname);
        nasm_free(oldline);
    }
    if (tasm_compatible_mode)
        return check_tasm_directive(line);
    return line;
}