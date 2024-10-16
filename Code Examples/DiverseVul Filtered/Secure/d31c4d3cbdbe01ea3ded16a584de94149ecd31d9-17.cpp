R_API RBinSection *r_bin_get_section_at(RBinObject *o, ut64 off, int va) {
	RBinSection *section;
	RListIter *iter;
	ut64 from, to;
	if (o) {
		// TODO: must be O(1) .. use sdb here
		r_list_foreach (o->sections, iter, section) {
			from = va? binobj_a2b (o, section->vaddr): section->paddr;
			to = va? (binobj_a2b (o, section->vaddr) + section->vsize) :
				(section->paddr + section->size);
			if (off >= from && off < to) {
				return section;
			}
		}
	}
	return NULL;
}