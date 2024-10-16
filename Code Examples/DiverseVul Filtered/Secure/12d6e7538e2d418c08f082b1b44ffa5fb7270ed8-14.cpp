bool kvm_is_mmio_pfn(pfn_t pfn)
{
	if (pfn_valid(pfn)) {
		int reserved;
		struct page *tail = pfn_to_page(pfn);
		struct page *head = compound_trans_head(tail);
		reserved = PageReserved(head);
		if (head != tail) {
			/*
			 * "head" is not a dangling pointer
			 * (compound_trans_head takes care of that)
			 * but the hugepage may have been splitted
			 * from under us (and we may not hold a
			 * reference count on the head page so it can
			 * be reused before we run PageReferenced), so
			 * we've to check PageTail before returning
			 * what we just read.
			 */
			smp_rmb();
			if (PageTail(tail))
				return reserved;
		}
		return PageReserved(tail);
	}

	return true;
}