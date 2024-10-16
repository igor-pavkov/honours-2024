static int ringbuf_map_mmap(struct bpf_map *map, struct vm_area_struct *vma)
{
	struct bpf_ringbuf_map *rb_map;
	size_t mmap_sz;

	rb_map = container_of(map, struct bpf_ringbuf_map, map);
	mmap_sz = bpf_ringbuf_mmap_page_cnt(rb_map->rb) << PAGE_SHIFT;

	if (vma->vm_pgoff * PAGE_SIZE + (vma->vm_end - vma->vm_start) > mmap_sz)
		return -EINVAL;

	return remap_vmalloc_range(vma, rb_map->rb,
				   vma->vm_pgoff + RINGBUF_PGOFF);
}