static size_t bpf_ringbuf_rec_pg_off(struct bpf_ringbuf *rb,
				     struct bpf_ringbuf_hdr *hdr)
{
	return ((void *)hdr - (void *)rb) >> PAGE_SHIFT;
}