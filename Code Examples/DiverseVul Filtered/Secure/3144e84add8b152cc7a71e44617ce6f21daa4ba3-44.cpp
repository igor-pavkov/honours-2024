static void free_llist(Line * list)
{
    Line *l, *tmp;
    list_for_each_safe(l, tmp, list) {
        free_tlist(l->first);
        nasm_free(l);
    }
}