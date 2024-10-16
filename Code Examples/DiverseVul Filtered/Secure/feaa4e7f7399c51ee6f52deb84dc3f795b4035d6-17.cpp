static void process_constructors(RKernelCacheObj *obj, struct MACH0_(obj_t) *mach0, RList *ret, ut64 paddr, bool is_first, int mode, const char *prefix) {
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (mach0))) {
		return;
	}
	int i, type;
	for (i = 0; !sections[i].last; i++) {
		if (sections[i].size == 0) {
			continue;
		}

		if (strstr (sections[i].name, "_mod_fini_func") || strstr (sections[i].name, "_mod_term_func")) {
			type  = R_BIN_ENTRY_TYPE_FINI;
		} else if (strstr (sections[i].name, "_mod_init_func")) {
			type  = is_first ? 0 : R_BIN_ENTRY_TYPE_INIT;
			is_first = false;
		} else {
			continue;
		}

		ut8 *buf = calloc (sections[i].size, 1);
		if (!buf) {
			break;
		}
		if (r_buf_read_at (obj->cache_buf, sections[i].offset + paddr, buf, sections[i].size) < sections[i].size) {
			free (buf);
			break;
		}
		int j;
		int count = 0;
		for (j = 0; j < sections[i].size; j += 8) {
			ut64 addr64 = K_RPTR (buf + j);
			ut64 paddr64 = sections[i].offset + paddr + j;
			if (mode == R_K_CONSTRUCTOR_TO_ENTRY) {
				RBinAddr *ba = newEntry (paddr64, addr64, type);
				r_list_append (ret, ba);
			} else if (mode == R_K_CONSTRUCTOR_TO_SYMBOL) {
				RBinSymbol *sym = R_NEW0 (RBinSymbol);
				if (!sym) {
					break;
				}

				sym->name = r_str_newf ("%s.%s.%d", prefix, (type == R_BIN_ENTRY_TYPE_INIT) ? "init" : "fini", count++);
				sym->vaddr = addr64;
				sym->paddr = paddr64;
				sym->size = 0;
				sym->forwarder = "NONE";
				sym->bind = "GLOBAL";
				sym->type = "FUNC";

				r_list_append (ret, sym);
			}
		}
		free (buf);
	}
	free (sections);
}