static FILE *inc_fopen(const char *file,
                       StrList **dhead,
                       const char **found_path,
                       enum incopen_mode omode,
                       enum file_flags fmode)
{
    StrList *sl;
    struct hash_insert hi;
    void **hp;
    char *path;
    FILE *fp = NULL;

    hp = hash_find(&FileHash, file, &hi);
    if (hp) {
        path = *hp;
        if (path || omode != INC_NEEDED) {
            nasm_add_string_to_strlist(dhead, path ? path : file);
        }
    } else {
        /* Need to do the actual path search */
        size_t file_len;

        sl = NULL;
        fp = inc_fopen_search(file, &sl, omode, fmode);

        file_len = strlen(file);

        if (!sl) {
            /* Store negative result for this file */
            sl = nasm_malloc(file_len + 1 + sizeof sl->next);
            memcpy(sl->str, file, file_len+1);
            sl->next = NULL;
            file = sl->str;
            path = NULL;
        } else  {
            path = sl->str;
            file = strchr(path, '\0') - file_len;
        }

        hash_add(&hi, file, path); /* Positive or negative result */

        /*
         * Add file to dependency path. The in_list() is needed
         * in case the file was already added with %depend.
         */
        if (path || omode != INC_NEEDED)
            nasm_add_to_strlist(dhead, sl);
    }

    if (!path) {
        if (omode == INC_NEEDED)
            nasm_fatal(0, "unable to open include file `%s'", file);

        if (found_path)
            *found_path = NULL;

        return NULL;
    }

    if (!fp && omode != INC_PROBE)
        fp = nasm_open_read(path, fmode);

    if (found_path)
        *found_path = path;

    return fp;
}