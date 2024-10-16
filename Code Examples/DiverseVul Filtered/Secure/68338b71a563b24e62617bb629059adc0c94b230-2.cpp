static int already_entry(RList *entries, ut64 vaddr) {
	RBinAddr *e;
	RListIter *iter;
	r_list_foreach (entries, iter, e) {
		if (e->vaddr == vaddr) {
			return 1;
		}
	}
	return 0;
}