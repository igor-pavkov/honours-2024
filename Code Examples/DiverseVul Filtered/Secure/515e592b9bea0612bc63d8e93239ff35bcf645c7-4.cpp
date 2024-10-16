static RBinSection *bin_section_from_segment(RCoreSymCacheElementSegment *seg) {
	if (!seg->name) {
		return NULL;
	}
	RBinSection *s = R_NEW0 (RBinSection);
	if (!s) {
		return NULL;
	}
	s->name = r_str_ndup (seg->name, 16);
	s->size = seg->size;
	s->vsize = seg->vsize;
	s->paddr = seg->paddr;
	s->vaddr = seg->vaddr;
	s->add = true;
	s->perm = strstr (s->name, "TEXT") ? 5 : 4;
	s->is_segment = true;
	return s;
}