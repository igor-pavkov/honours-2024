static RBinSection *bin_section_from_section(RCoreSymCacheElementSection *sect) {
	if (!sect->name) {
		return NULL;
	}
	RBinSection *s = R_NEW0 (RBinSection);
	if (!s) {
		return NULL;
	}
	s->name = r_str_ndup (sect->name, 256);
	s->size = sect->size;
	s->vsize = s->size;
	s->paddr = sect->paddr;
	s->vaddr = sect->vaddr;
	s->add = true;
	s->perm = strstr (s->name, "TEXT") ? 5 : 4;
	s->is_segment = false;
	return s;
}