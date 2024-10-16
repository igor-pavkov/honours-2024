static char *get_param(char *p, char *q, bool *advance)
{
    *advance = false;
    if (p[2]) /* the parameter's in the option */
        return nasm_skip_spaces(p + 2);
    if (q && q[0]) {
        *advance = true;
        return q;
    }
    nasm_error(ERR_NONFATAL | ERR_NOFILE | ERR_USAGE,
                 "option `-%c' requires an argument", p[1]);
    return NULL;
}