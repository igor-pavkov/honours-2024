static void copy_filename(const char **dst, const char *src, const char *what)
{
    if (*dst)
        nasm_fatal(0, "more than one %s file specified: %s\n", what, src);

    *dst = nasm_strdup(src);
}