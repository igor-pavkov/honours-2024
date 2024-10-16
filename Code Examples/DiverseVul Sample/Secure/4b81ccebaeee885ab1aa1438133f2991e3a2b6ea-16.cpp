BPF_CALL_3(bpf_ringbuf_reserve, struct bpf_map *, map, u64, size, u64, flags)
{
	struct bpf_ringbuf_map *rb_map;

	if (unlikely(flags))
		return 0;

	rb_map = container_of(map, struct bpf_ringbuf_map, map);
	return (unsigned long)__bpf_ringbuf_reserve(rb_map->rb, size);
}