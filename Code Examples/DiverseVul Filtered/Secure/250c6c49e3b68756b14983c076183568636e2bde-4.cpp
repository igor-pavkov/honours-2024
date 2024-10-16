int sbusfb_mmap_helper(struct sbus_mmap_map *map,
		       unsigned long physbase,
		       unsigned long fbsize,
		       unsigned long iospace,
		       struct vm_area_struct *vma)
{
	unsigned int size, page, r, map_size;
	unsigned long map_offset = 0;
	unsigned long off;
	int i;
                                        
	if (!(vma->vm_flags & (VM_SHARED | VM_MAYSHARE)))
		return -EINVAL;

	size = vma->vm_end - vma->vm_start;
	if (vma->vm_pgoff > (~0UL >> PAGE_SHIFT))
		return -EINVAL;

	off = vma->vm_pgoff << PAGE_SHIFT;

	/* VM_IO | VM_DONTEXPAND | VM_DONTDUMP are set by remap_pfn_range() */

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	/* Each page, see which map applies */
	for (page = 0; page < size; ){
		map_size = 0;
		for (i = 0; map[i].size; i++)
			if (map[i].voff == off+page) {
				map_size = sbusfb_mmapsize(map[i].size, fbsize);
#ifdef __sparc_v9__
#define POFF_MASK	(PAGE_MASK|0x1UL)
#else
#define POFF_MASK	(PAGE_MASK)
#endif				
				map_offset = (physbase + map[i].poff) & POFF_MASK;
				break;
			}
		if (!map_size) {
			page += PAGE_SIZE;
			continue;
		}
		if (page + map_size > size)
			map_size = size - page;
		r = io_remap_pfn_range(vma,
					vma->vm_start + page,
					MK_IOSPACE_PFN(iospace,
						map_offset >> PAGE_SHIFT),
					map_size,
					vma->vm_page_prot);
		if (r)
			return -EAGAIN;
		page += map_size;
	}

	return 0;
}