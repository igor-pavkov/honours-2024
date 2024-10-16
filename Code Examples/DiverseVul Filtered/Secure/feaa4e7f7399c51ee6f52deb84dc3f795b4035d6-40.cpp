static RStubsInfo *get_stubs_info(struct MACH0_(obj_t) *mach0, ut64 paddr, RKernelCacheObj *obj) {
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (mach0))) {
		return NULL;
	}

	RStubsInfo *stubs_info = R_NEW0 (RStubsInfo);
	if (!stubs_info) {
		free (sections);
		return NULL;
	}

	int incomplete = 2;
	int i = 0;
	for (; !sections[i].last; i++) {
		if (strstr (sections[i].name, "__DATA_CONST.__got")) {
			stubs_info->got.offset = sections[i].offset + paddr;
			stubs_info->got.size = sections[i].size;
			stubs_info->got_addr = K_PPTR (sections[i].addr);
			if (!--incomplete) {
				break;
			}
		}

		if (strstr (sections[i].name, "__TEXT_EXEC.__stubs")) {
			stubs_info->stubs.offset = sections[i].offset + paddr;
			stubs_info->stubs.size = sections[i].size;
			if (!--incomplete) {
				break;
			}
		}
	}

	R_FREE (sections);

	if (incomplete) {
		R_FREE (stubs_info);
	}

	return stubs_info;
}