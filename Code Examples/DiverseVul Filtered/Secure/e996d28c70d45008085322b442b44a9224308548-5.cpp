static void emit_dependencies(StrList *list)
{
    FILE *deps;
    int linepos, len;
    StrList *l, *nl;
    bool wmake = (quote_for_make == quote_for_wmake);
    const char *wrapstr, *nulltarget;

    wrapstr = wmake ? " &\n " : " \\\n ";
    nulltarget = wmake ? "\t%null\n" : "";

    if (depend_file && strcmp(depend_file, "-")) {
        deps = nasm_open_write(depend_file, NF_TEXT);
        if (!deps) {
            nasm_error(ERR_NONFATAL|ERR_NOFILE|ERR_USAGE,
                       "unable to write dependency file `%s'", depend_file);
            return;
        }
    } else {
        deps = stdout;
    }

    linepos = fprintf(deps, "%s :", depend_target);
    list_for_each(l, list) {
        char *file = quote_for_make(l->str);
        len = strlen(file);
        if (linepos + len > 62 && linepos > 1) {
            fputs(wrapstr, deps);
            linepos = 1;
        }
        fprintf(deps, " %s", file);
        linepos += len+1;
        nasm_free(file);
    }
    fprintf(deps, "\n\n");

    list_for_each_safe(l, nl, list) {
        if (depend_emit_phony) {
            char *file = quote_for_make(l->str);
            fprintf(deps, "%s :\n%s\n", file, nulltarget);
            nasm_free(file);
        }
        nasm_free(l);
    }

    if (deps != stdout)
        fclose(deps);
}