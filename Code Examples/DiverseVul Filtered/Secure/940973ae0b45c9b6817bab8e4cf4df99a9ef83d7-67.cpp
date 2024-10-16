static void dma_buf_commit(IDEState *s)
{
    qemu_sglist_destroy(&s->sg);
}