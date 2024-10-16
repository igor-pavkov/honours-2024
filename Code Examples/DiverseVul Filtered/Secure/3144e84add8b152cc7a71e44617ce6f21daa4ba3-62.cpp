static size_t nasm_unquote_cstr(char *qstr, enum preproc_token directive)
{
    size_t len = nasm_unquote(qstr, NULL);
    size_t clen = strlen(qstr);

    if (len != clen)
        nasm_error(ERR_NONFATAL, "NUL character in `%s' directive",
              pp_directives[directive]);

    return clen;
}