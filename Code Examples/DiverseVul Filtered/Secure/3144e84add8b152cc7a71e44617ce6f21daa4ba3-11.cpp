static void *new_Block(size_t size)
{
    Blocks *b = &blocks;

    /* first, get to the end of the linked list */
    while (b->next)
        b = b->next;
    /* now allocate the requested chunk */
    b->chunk = nasm_malloc(size);

    /* now allocate a new block for the next request */
    b->next = nasm_zalloc(sizeof(Blocks));
    return b->chunk;
}