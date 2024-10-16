static void bpf_ringbuf_free(struct bpf_ringbuf *rb)
{
	/* copy pages pointer and nr_pages to local variable, as we are going
	 * to unmap rb itself with vunmap() below
	 */
	struct page **pages = rb->pages;
	int i, nr_pages = rb->nr_pages;

	vunmap(rb);
	for (i = 0; i < nr_pages; i++)
		__free_page(pages[i]);
	kvfree(pages);
}