static __poll_t ringbuf_map_poll(struct bpf_map *map, struct file *filp,
				 struct poll_table_struct *pts)
{
	struct bpf_ringbuf_map *rb_map;

	rb_map = container_of(map, struct bpf_ringbuf_map, map);
	poll_wait(filp, &rb_map->rb->waitq, pts);

	if (ringbuf_avail_data_sz(rb_map->rb))
		return EPOLLIN | EPOLLRDNORM;
	return 0;
}