static void free_mmacro(MMacro * m)
{
    nasm_free(m->name);
    free_tlist(m->dlist);
    nasm_free(m->defaults);
    free_llist(m->expansion);
    nasm_free(m);
}