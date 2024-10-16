static int hns_roce_mmap(struct ib_ucontext *context,
			 struct vm_area_struct *vma)
{
	struct hns_roce_dev *hr_dev = to_hr_dev(context->device);

	if (((vma->vm_end - vma->vm_start) % PAGE_SIZE) != 0)
		return -EINVAL;

	if (vma->vm_pgoff == 0) {
		vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
		if (io_remap_pfn_range(vma, vma->vm_start,
				       to_hr_ucontext(context)->uar.pfn,
				       PAGE_SIZE, vma->vm_page_prot))
			return -EAGAIN;
	} else if (vma->vm_pgoff == 1 && hr_dev->tptr_dma_addr &&
		   hr_dev->tptr_size) {
		/* vm_pgoff: 1 -- TPTR */
		if (io_remap_pfn_range(vma, vma->vm_start,
				       hr_dev->tptr_dma_addr >> PAGE_SHIFT,
				       hr_dev->tptr_size,
				       vma->vm_page_prot))
			return -EAGAIN;
	} else
		return -EINVAL;

	return 0;
}