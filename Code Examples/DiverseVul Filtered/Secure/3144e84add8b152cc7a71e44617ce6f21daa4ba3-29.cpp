static void free_mmacro_table(struct hash_table *mmt)
{
    MMacro *m, *tmp;
    const char *key;
    struct hash_tbl_node *it = NULL;

    it = NULL;
    while ((m = hash_iterate(mmt, &it, &key)) != NULL) {
        nasm_free((void *)key);
        list_for_each_safe(m ,tmp, m)
            free_mmacro(m);
    }
    hash_free(mmt);
}