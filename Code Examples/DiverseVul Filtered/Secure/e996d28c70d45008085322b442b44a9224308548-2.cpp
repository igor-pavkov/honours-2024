static void nasm_verror_gnu(int severity, const char *fmt, va_list ap)
{
    const char *currentfile = NULL;
    int32_t lineno = 0;

    if (is_suppressed_warning(severity))
        return;

    if (!(severity & ERR_NOFILE)) {
	src_get(&lineno, &currentfile);
        if (!currentfile || (severity & ERR_TOPFILE)) {
            currentfile = inname[0] ? inname : outname[0] ? outname : NULL;
            lineno = 0;
        }
    }

    if (!skip_this_pass(severity)) {
        if (!lineno)
            fprintf(error_file, "%s:", currentfile ? currentfile : "nasm");
        else
            fprintf(error_file, "%s:%"PRId32": ", currentfile, lineno);
    }

    nasm_verror_common(severity, fmt, ap);
}