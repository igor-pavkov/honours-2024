static int usbvision_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long size = vma->vm_end - vma->vm_start,
		start = vma->vm_start;
	void *pos;
	u32 i;
	struct usb_usbvision *usbvision = video_drvdata(file);

	PDEBUG(DBG_MMAP, "mmap");

	if (!USBVISION_IS_OPERATIONAL(usbvision))
		return -EFAULT;

	if (!(vma->vm_flags & VM_WRITE) ||
	    size != PAGE_ALIGN(usbvision->max_frame_size)) {
		return -EINVAL;
	}

	for (i = 0; i < usbvision->num_frames; i++) {
		if (((PAGE_ALIGN(usbvision->max_frame_size)*i) >> PAGE_SHIFT) ==
		    vma->vm_pgoff)
			break;
	}
	if (i == usbvision->num_frames) {
		PDEBUG(DBG_MMAP,
		       "mmap: user supplied mapping address is out of range");
		return -EINVAL;
	}

	/* VM_IO is eventually going to replace PageReserved altogether */
	vma->vm_flags |= VM_IO | VM_DONTEXPAND | VM_DONTDUMP;

	pos = usbvision->frame[i].data;
	while (size > 0) {
		if (vm_insert_page(vma, start, vmalloc_to_page(pos))) {
			PDEBUG(DBG_MMAP, "mmap: vm_insert_page failed");
			return -EAGAIN;
		}
		start += PAGE_SIZE;
		pos += PAGE_SIZE;
		size -= PAGE_SIZE;
	}

	return 0;
}