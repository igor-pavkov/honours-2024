static FILE *inc_fopen_search(const char *file, StrList **slpath,
                              enum incopen_mode omode, enum file_flags fmode)
{
    FILE *fp;
    char *prefix = "";
    const IncPath *ip = ipath;
    int len = strlen(file);
    size_t prefix_len = 0;
    StrList *sl;
    size_t path_len;
    bool found;

    while (1) {
        path_len = prefix_len + len + 1;

        sl = nasm_malloc(path_len + sizeof sl->next);
        memcpy(sl->str, prefix, prefix_len);
        memcpy(sl->str+prefix_len, file, len+1);
        sl->next = NULL;

        if (omode == INC_PROBE) {
            fp = NULL;
            found = nasm_file_exists(sl->str);
        } else {
            fp = nasm_open_read(sl->str, fmode);
            found = (fp != NULL);
        }
        if (found) {
            *slpath = sl;
            return fp;
        }

        nasm_free(sl);

        if (!ip)
            return NULL;

        prefix = ip->path;
        prefix_len = strlen(prefix);
        ip = ip->next;
    }
}