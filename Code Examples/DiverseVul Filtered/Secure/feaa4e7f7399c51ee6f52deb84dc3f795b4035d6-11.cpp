static void r_kext_fill_text_range(RKext *kext) {
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (kext->mach0))) {
		return;
	}

	int i = 0;
	for (; !sections[i].last; i++) {
		if (strstr (sections[i].name, "__TEXT_EXEC.__text")) {
			kext->text_range.offset = sections[i].offset;
			kext->text_range.size = sections[i].size;
			kext->vaddr = sections[i].addr;
			break;
		}
	}

	R_FREE (sections);
}