static void free_smacro_table(struct hash_table *smt)
{
    SMacro *s, *tmp;
    const char *key;
    struct hash_tbl_node *it = NULL;

    while ((s = hash_iterate(smt, &it, &key)) != NULL) {
        nasm_free((void *)key);
        list_for_each_safe(s, tmp, s) {
            nasm_free(s->name);
            free_tlist(s->expansion);
            nasm_free(s);
        }
    }
    hash_free(smt);
}