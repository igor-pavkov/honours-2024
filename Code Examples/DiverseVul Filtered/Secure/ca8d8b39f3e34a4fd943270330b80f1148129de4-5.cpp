RList *MACH0_(get_segments)(RBinFile *bf) {
	struct MACH0_(obj_t) *bin = bf->o->bin_obj;
	RList *list = r_list_newf ((RListFree)r_bin_section_free);
	size_t i, j;

	/* for core files */
	if (bin->nsegs > 0) {
		struct MACH0_(segment_command) *seg;
		for (i = 0; i < bin->nsegs; i++) {
			seg = &bin->segs[i];
			if (!seg->initprot) {
				continue;
			}
			RBinSection *s = r_bin_section_new (NULL);
			if (!s) {
				break;
			}
			s->vaddr = seg->vmaddr;
			s->vsize = seg->vmsize;
			s->size = seg->vmsize;
			s->paddr = seg->fileoff;
			s->paddr += bf->o->boffset;
			//TODO s->flags = seg->flags;
			s->name = r_str_ndup (seg->segname, 16);
			s->is_segment = true;
			r_str_filter (s->name, -1);
			s->perm = prot2perm (seg->initprot);
			s->add = true;
			r_list_append (list, s);
		}
	}
	if (bin->nsects > 0) {
		int last_section = R_MIN (bin->nsects, 128); // maybe drop this limit?
		for (i = 0; i < last_section; i++) {
			RBinSection *s = R_NEW0 (RBinSection);
			if (!s) {
				break;
			}
			s->vaddr = (ut64)bin->sects[i].addr;
			s->vsize = (ut64)bin->sects[i].size;
			s->is_segment = false;
			s->size = (bin->sects[i].flags == S_ZEROFILL) ? 0 : (ut64)bin->sects[i].size;
			// XXX flags
			s->paddr = (ut64)bin->sects[i].offset;
			int segment_index = 0;
			//s->perm =prot2perm (bin->segs[j].initprot);
			for (j = 0; j < bin->nsegs; j++) {
				if (s->vaddr >= bin->segs[j].vmaddr &&
						s->vaddr < (bin->segs[j].vmaddr + bin->segs[j].vmsize)) {
					s->perm = prot2perm (bin->segs[j].initprot);
					segment_index = j;
					break;
				}
			}
			char *section_name = r_str_ndup (bin->sects[i].sectname, 16);
			char *segment_name = r_str_newf ("%u.%s", (ut32)i, bin->segs[segment_index].segname);
			s->name = r_str_newf ("%s.%s", segment_name, section_name);
			if (strstr (s->name, "__const")) {
				s->format = r_str_newf ("Cd 4[%"PFMT64d"]", s->size / 4);
			}
			s->is_data = is_data_section (s);
			if (strstr (section_name, "interpos") || strstr (section_name, "__mod_")) {
#if R_BIN_MACH064
				const int ws = 8;
#else
				const int ws = 4;
#endif
				s->format = r_str_newf ("Cd %d[%"PFMT64d"]", ws, s->vsize / ws);
			}
			r_list_append (list, s);
			free (segment_name);
			free (section_name);
		}
	}
	return list;
}