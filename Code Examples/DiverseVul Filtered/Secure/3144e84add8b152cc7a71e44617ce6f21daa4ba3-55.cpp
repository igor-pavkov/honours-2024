static void delete_Blocks(void)
{
    Blocks *a, *b = &blocks;

    /*
     * keep in mind that the first block, pointed to by blocks
     * is a static and not dynamically allocated, so we don't
     * free it.
     */
    while (b) {
        if (b->chunk)
            nasm_free(b->chunk);
        a = b;
        b = b->next;
        if (a != &blocks)
            nasm_free(a);
    }
    memset(&blocks, 0, sizeof(blocks));
}