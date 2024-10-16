static void process_kmod_init_term(RKernelCacheObj *obj, RKext *kext, RList *ret, ut64 **inits, ut64 **terms) {
	if (!*inits || !*terms) {
		struct section_t *sections = NULL;
		if (!(sections = MACH0_(get_sections) (obj->mach0))) {
			return;
		}

		int i = 0;
		for (; !sections[i].last; i++) {
			if (sections[i].size == 0) {
				continue;
			}

			ut64 start_paddr = 0;
			ut64 *target = NULL;
			int n_ptrs = 0;

			if (!*inits && strstr (sections[i].name, "__kmod_init")) {
				int n_inits = sections[i].size / 8;
				if (n_inits <= 0) {
					continue;
				}
				*inits = R_NEWS0 (ut64, n_inits + 1);
				target = *inits;
				n_ptrs = n_inits;
			}
			if (!*terms && strstr (sections[i].name, "__kmod_term")) {
				int n_terms = sections[i].size / 8;
				if (n_terms <= 0) {
					continue;
				}
				*terms = R_NEWS0 (ut64, n_terms + 1);
				target = *terms;
				n_ptrs = n_terms;
			}
			if (!target || !n_ptrs) {
				continue;
			}
			start_paddr = sections[i].offset;
			int j = 0;
			ut8 bytes[8];
			for (; j < n_ptrs; j++) {
				if (r_buf_read_at (obj->cache_buf, start_paddr + j * 8, bytes, 8) < 8) {
					break;
				}
				target[j] = K_RPTR (bytes);
			}
			target[j] = 0;
		}

		R_FREE (sections);
	}

	if (*inits) {
		create_initterm_syms (kext, ret, R_BIN_ENTRY_TYPE_INIT, *inits);
	}
	if (*terms) {
		create_initterm_syms (kext, ret, R_BIN_ENTRY_TYPE_FINI, *terms);
	}
}