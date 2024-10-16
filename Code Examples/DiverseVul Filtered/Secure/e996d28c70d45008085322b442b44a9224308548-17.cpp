static void nasm_verror_vc(int severity, const char *fmt, va_list ap)
{
    const char *currentfile = NULL;
    int32_t lineno = 0;

    if (is_suppressed_warning(severity))
        return;

    if (!(severity & ERR_NOFILE))
        src_get(&lineno, &currentfile);

    if (!skip_this_pass(severity)) {
        if (currentfile) {
	    fprintf(error_file, "%s(%"PRId32") : ", currentfile, lineno);
	} else {
	    fputs("nasm: ", error_file);
	}
    }

    nasm_verror_common(severity, fmt, ap);
}