static RList *sections(RBinFile *bf) {
	RList *res = r_list_newf ((RListFree)r_bin_section_free);
	r_return_val_if_fail (res && bf->o && bf->o->bin_obj, res);
	RCoreSymCacheElement *element = bf->o->bin_obj;
	size_t i;
	if (element->segments) {
		for (i = 0; i < element->hdr->n_segments; i++) {
			RCoreSymCacheElementSegment *seg = &element->segments[i];
			RBinSection *s = bin_section_from_segment (seg);
			if (s) {
				r_list_append (res, s);
			}
		}
	}
	if (element->sections) {
		for (i = 0; i < element->hdr->n_sections; i++) {
			RCoreSymCacheElementSection *sect = &element->sections[i];
			RBinSection *s = bin_section_from_section (sect);
			if (s) {
				r_list_append (res, s);
			}
		}
	}
	return res;
}