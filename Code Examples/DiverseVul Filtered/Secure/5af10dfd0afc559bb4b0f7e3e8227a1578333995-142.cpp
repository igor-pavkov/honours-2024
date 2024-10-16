u32 hugetlb_fault_mutex_hash(struct hstate *h, struct mm_struct *mm,
			    struct vm_area_struct *vma,
			    struct address_space *mapping,
			    pgoff_t idx, unsigned long address)
{
	return 0;
}