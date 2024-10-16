static size_t bpf_ringbuf_mmap_page_cnt(const struct bpf_ringbuf *rb)
{
	size_t data_pages = (rb->mask + 1) >> PAGE_SHIFT;

	/* consumer page + producer page + 2 x data pages */
	return RINGBUF_POS_PAGES + 2 * data_pages;
}