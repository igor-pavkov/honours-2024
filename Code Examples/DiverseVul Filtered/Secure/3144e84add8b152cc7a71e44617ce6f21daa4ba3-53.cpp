static Context *get_ctx(const char *name, const char **namep)
{
    Context *ctx;
    int i;

    if (namep)
        *namep = name;

    if (!name || name[0] != '%' || name[1] != '$')
        return NULL;

    if (!cstk) {
        nasm_error(ERR_NONFATAL, "`%s': context stack is empty", name);
        return NULL;
    }

    name += 2;
    ctx = cstk;
    i = 0;
    while (ctx && *name == '$') {
        name++;
        i++;
        ctx = ctx->next;
    }
    if (!ctx) {
        nasm_error(ERR_NONFATAL, "`%s': context stack is only"
              " %d level%s deep", name, i, (i == 1 ? "" : "s"));
        return NULL;
    }

    if (namep)
        *namep = name;

    return ctx;
}