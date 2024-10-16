static void sections_from_mach0(RList *ret, struct MACH0_(obj_t) *mach0, RBinFile *bf, ut64 paddr, char *prefix, RKernelCacheObj *obj) {
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (mach0))) {
		return;
	}
	int i;
	for (i = 0; !sections[i].last; i++) {
		RBinSection *ptr;
		if (!(ptr = R_NEW0 (RBinSection))) {
			break;
		}
		if (prefix) {
			ptr->name = r_str_newf ("%s.%s", prefix, (char*)sections[i].name);
		} else {
			ptr->name = r_str_newf ("%s", (char*)sections[i].name);
		}
		if (strstr (ptr->name, "la_symbol_ptr")) {
			int len = sections[i].size / 8;
			ptr->format = r_str_newf ("Cd %d[%d]", 8, len);
		}
		handle_data_sections (ptr);
		ptr->size = sections[i].size;
		ptr->vsize = sections[i].vsize;
		ptr->paddr = sections[i].offset + bf->o->boffset + paddr;
		ptr->vaddr = K_PPTR (sections[i].addr);
		if (!ptr->vaddr) {
			ptr->vaddr = ptr->paddr;
		}
		ptr->perm = sections[i].perm;
		if (!ptr->perm && strstr (sections[i].name, "__TEXT_EXEC.__text")) {
			ptr->perm = 1 | 4;
		}
		r_list_append (ret, ptr);
	}
	free (sections);
}