static void pp_include_path(char *path)
{
    IncPath *i;

    i = nasm_malloc(sizeof(IncPath));
    i->path = path ? nasm_strdup(path) : NULL;
    i->next = NULL;

    if (ipath) {
        IncPath *j = ipath;
        while (j->next)
            j = j->next;
        j->next = i;
    } else {
        ipath = i;
    }
}