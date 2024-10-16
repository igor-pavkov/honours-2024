static int vhost_vdpa_va_map(struct vhost_vdpa *v,
			     u64 iova, u64 size, u64 uaddr, u32 perm)
{
	struct vhost_dev *dev = &v->vdev;
	u64 offset, map_size, map_iova = iova;
	struct vdpa_map_file *map_file;
	struct vm_area_struct *vma;
	int ret = 0;

	mmap_read_lock(dev->mm);

	while (size) {
		vma = find_vma(dev->mm, uaddr);
		if (!vma) {
			ret = -EINVAL;
			break;
		}
		map_size = min(size, vma->vm_end - uaddr);
		if (!(vma->vm_file && (vma->vm_flags & VM_SHARED) &&
			!(vma->vm_flags & (VM_IO | VM_PFNMAP))))
			goto next;

		map_file = kzalloc(sizeof(*map_file), GFP_KERNEL);
		if (!map_file) {
			ret = -ENOMEM;
			break;
		}
		offset = (vma->vm_pgoff << PAGE_SHIFT) + uaddr - vma->vm_start;
		map_file->offset = offset;
		map_file->file = get_file(vma->vm_file);
		ret = vhost_vdpa_map(v, map_iova, map_size, uaddr,
				     perm, map_file);
		if (ret) {
			fput(map_file->file);
			kfree(map_file);
			break;
		}
next:
		size -= map_size;
		uaddr += map_size;
		map_iova += map_size;
	}
	if (ret)
		vhost_vdpa_unmap(v, iova, map_iova - iova);

	mmap_read_unlock(dev->mm);

	return ret;
}