BPF_CALL_4(bpf_ringbuf_output, struct bpf_map *, map, void *, data, u64, size,
	   u64, flags)
{
	struct bpf_ringbuf_map *rb_map;
	void *rec;

	if (unlikely(flags & ~(BPF_RB_NO_WAKEUP | BPF_RB_FORCE_WAKEUP)))
		return -EINVAL;

	rb_map = container_of(map, struct bpf_ringbuf_map, map);
	rec = __bpf_ringbuf_reserve(rb_map->rb, size);
	if (!rec)
		return -EAGAIN;

	memcpy(rec, data, size);
	bpf_ringbuf_commit(rec, flags, false /* discard */);
	return 0;
}