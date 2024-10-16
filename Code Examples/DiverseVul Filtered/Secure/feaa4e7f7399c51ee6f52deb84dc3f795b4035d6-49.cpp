static RPrelinkRange *get_prelink_info_range_from_mach0(struct MACH0_(obj_t) *mach0) {
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (mach0))) {
		return NULL;
	}

	RPrelinkRange *prelink_range = R_NEW0 (RPrelinkRange);
	if (!prelink_range) {
		R_FREE (sections);
		return NULL;
	}

	int incomplete = 3;
	int i = 0;
	for (; !sections[i].last; i++) {
		if (strstr (sections[i].name, "__PRELINK_INFO.__info")) {
			prelink_range->range.offset = sections[i].offset;
			prelink_range->range.size = sections[i].size;
			if (!--incomplete) {
				break;
			}
		}

		if (strstr (sections[i].name, "__PRELINK_TEXT.__text")) {
			prelink_range->pa2va_exec = sections[i].addr - sections[i].offset;
			if (!--incomplete) {
				break;
			}
		}

		if (strstr (sections[i].name, "__PRELINK_DATA.__data")) {
			prelink_range->pa2va_data = sections[i].addr - sections[i].offset;
			if (!--incomplete) {
				break;
			}
		}
	}

	R_FREE (sections);

	if (incomplete == 1 && !prelink_range->pa2va_data) {
		struct MACH0_(segment_command) *seg;
		int nsegs = R_MIN (mach0->nsegs, 128);
		size_t i;
		for (i = 0; i < nsegs; i++) {
			seg = &mach0->segs[i];
			if (!strcmp (seg->segname, "__DATA")) {
				prelink_range->pa2va_data = seg->vmaddr - seg->fileoff;
				incomplete--;
				break;
			}
		}
	}

	if (incomplete) {
		R_FREE (prelink_range);
	}

	return prelink_range;
}