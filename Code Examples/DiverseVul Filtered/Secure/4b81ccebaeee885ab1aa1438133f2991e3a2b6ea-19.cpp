static struct bpf_map *ringbuf_map_alloc(union bpf_attr *attr)
{
	struct bpf_ringbuf_map *rb_map;

	if (attr->map_flags & ~RINGBUF_CREATE_FLAG_MASK)
		return ERR_PTR(-EINVAL);

	if (attr->key_size || attr->value_size ||
	    !is_power_of_2(attr->max_entries) ||
	    !PAGE_ALIGNED(attr->max_entries))
		return ERR_PTR(-EINVAL);

#ifdef CONFIG_64BIT
	/* on 32-bit arch, it's impossible to overflow record's hdr->pgoff */
	if (attr->max_entries > RINGBUF_MAX_DATA_SZ)
		return ERR_PTR(-E2BIG);
#endif

	rb_map = kzalloc(sizeof(*rb_map), GFP_USER | __GFP_ACCOUNT);
	if (!rb_map)
		return ERR_PTR(-ENOMEM);

	bpf_map_init_from_attr(&rb_map->map, attr);

	rb_map->rb = bpf_ringbuf_alloc(attr->max_entries, rb_map->map.numa_node);
	if (!rb_map->rb) {
		kfree(rb_map);
		return ERR_PTR(-ENOMEM);
	}

	return &rb_map->map;
}