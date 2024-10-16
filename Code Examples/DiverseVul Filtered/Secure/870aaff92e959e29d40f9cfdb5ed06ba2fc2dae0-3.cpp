static void vhost_vdpa_pa_unmap(struct vhost_vdpa *v, u64 start, u64 last)
{
	struct vhost_dev *dev = &v->vdev;
	struct vhost_iotlb *iotlb = dev->iotlb;
	struct vhost_iotlb_map *map;
	struct page *page;
	unsigned long pfn, pinned;

	while ((map = vhost_iotlb_itree_first(iotlb, start, last)) != NULL) {
		pinned = PFN_DOWN(map->size);
		for (pfn = PFN_DOWN(map->addr);
		     pinned > 0; pfn++, pinned--) {
			page = pfn_to_page(pfn);
			if (map->perm & VHOST_ACCESS_WO)
				set_page_dirty_lock(page);
			unpin_user_page(page);
		}
		atomic64_sub(PFN_DOWN(map->size), &dev->mm->pinned_vm);
		vhost_iotlb_map_free(iotlb, map);
	}
}