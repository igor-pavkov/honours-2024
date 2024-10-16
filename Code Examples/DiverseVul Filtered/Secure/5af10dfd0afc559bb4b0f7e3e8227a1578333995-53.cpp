u32 hugetlb_fault_mutex_hash(struct hstate *h, struct mm_struct *mm,
			    struct vm_area_struct *vma,
			    struct address_space *mapping,
			    pgoff_t idx, unsigned long address)
{
	unsigned long key[2];
	u32 hash;

	if (vma->vm_flags & VM_SHARED) {
		key[0] = (unsigned long) mapping;
		key[1] = idx;
	} else {
		key[0] = (unsigned long) mm;
		key[1] = address >> huge_page_shift(h);
	}

	hash = jhash2((u32 *)&key, sizeof(key)/sizeof(u32), 0);

	return hash & (num_fault_mutexes - 1);
}