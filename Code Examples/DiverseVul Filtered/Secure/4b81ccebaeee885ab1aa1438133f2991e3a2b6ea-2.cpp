BPF_CALL_2(bpf_ringbuf_query, struct bpf_map *, map, u64, flags)
{
	struct bpf_ringbuf *rb;

	rb = container_of(map, struct bpf_ringbuf_map, map)->rb;

	switch (flags) {
	case BPF_RB_AVAIL_DATA:
		return ringbuf_avail_data_sz(rb);
	case BPF_RB_RING_SIZE:
		return rb->mask + 1;
	case BPF_RB_CONS_POS:
		return smp_load_acquire(&rb->consumer_pos);
	case BPF_RB_PROD_POS:
		return smp_load_acquire(&rb->producer_pos);
	default:
		return 0;
	}
}