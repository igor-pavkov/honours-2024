static int vhost_vdpa_pa_map(struct vhost_vdpa *v,
			     u64 iova, u64 size, u64 uaddr, u32 perm)
{
	struct vhost_dev *dev = &v->vdev;
	struct page **page_list;
	unsigned long list_size = PAGE_SIZE / sizeof(struct page *);
	unsigned int gup_flags = FOLL_LONGTERM;
	unsigned long npages, cur_base, map_pfn, last_pfn = 0;
	unsigned long lock_limit, sz2pin, nchunks, i;
	u64 start = iova;
	long pinned;
	int ret = 0;

	/* Limit the use of memory for bookkeeping */
	page_list = (struct page **) __get_free_page(GFP_KERNEL);
	if (!page_list)
		return -ENOMEM;

	if (perm & VHOST_ACCESS_WO)
		gup_flags |= FOLL_WRITE;

	npages = PFN_UP(size + (iova & ~PAGE_MASK));
	if (!npages) {
		ret = -EINVAL;
		goto free;
	}

	mmap_read_lock(dev->mm);

	lock_limit = PFN_DOWN(rlimit(RLIMIT_MEMLOCK));
	if (npages + atomic64_read(&dev->mm->pinned_vm) > lock_limit) {
		ret = -ENOMEM;
		goto unlock;
	}

	cur_base = uaddr & PAGE_MASK;
	iova &= PAGE_MASK;
	nchunks = 0;

	while (npages) {
		sz2pin = min_t(unsigned long, npages, list_size);
		pinned = pin_user_pages(cur_base, sz2pin,
					gup_flags, page_list, NULL);
		if (sz2pin != pinned) {
			if (pinned < 0) {
				ret = pinned;
			} else {
				unpin_user_pages(page_list, pinned);
				ret = -ENOMEM;
			}
			goto out;
		}
		nchunks++;

		if (!last_pfn)
			map_pfn = page_to_pfn(page_list[0]);

		for (i = 0; i < pinned; i++) {
			unsigned long this_pfn = page_to_pfn(page_list[i]);
			u64 csize;

			if (last_pfn && (this_pfn != last_pfn + 1)) {
				/* Pin a contiguous chunk of memory */
				csize = PFN_PHYS(last_pfn - map_pfn + 1);
				ret = vhost_vdpa_map(v, iova, csize,
						     PFN_PHYS(map_pfn),
						     perm, NULL);
				if (ret) {
					/*
					 * Unpin the pages that are left unmapped
					 * from this point on in the current
					 * page_list. The remaining outstanding
					 * ones which may stride across several
					 * chunks will be covered in the common
					 * error path subsequently.
					 */
					unpin_user_pages(&page_list[i],
							 pinned - i);
					goto out;
				}

				map_pfn = this_pfn;
				iova += csize;
				nchunks = 0;
			}

			last_pfn = this_pfn;
		}

		cur_base += PFN_PHYS(pinned);
		npages -= pinned;
	}

	/* Pin the rest chunk */
	ret = vhost_vdpa_map(v, iova, PFN_PHYS(last_pfn - map_pfn + 1),
			     PFN_PHYS(map_pfn), perm, NULL);
out:
	if (ret) {
		if (nchunks) {
			unsigned long pfn;

			/*
			 * Unpin the outstanding pages which are yet to be
			 * mapped but haven't due to vdpa_map() or
			 * pin_user_pages() failure.
			 *
			 * Mapped pages are accounted in vdpa_map(), hence
			 * the corresponding unpinning will be handled by
			 * vdpa_unmap().
			 */
			WARN_ON(!last_pfn);
			for (pfn = map_pfn; pfn <= last_pfn; pfn++)
				unpin_user_page(pfn_to_page(pfn));
		}
		vhost_vdpa_unmap(v, start, size);
	}
unlock:
	mmap_read_unlock(dev->mm);
free:
	free_page((unsigned long)page_list);
	return ret;

}