static RRebaseInfo *r_rebase_info_new_from_mach0(RBuffer *cache_buf, struct MACH0_(obj_t) *mach0) {
	RFileRange *rebase_ranges = NULL;
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (mach0))) {
		return NULL;
	}

	ut64 starts_offset = 0, starts_size = 0;

	int i = 0;
	for (; !sections[i].last; i++) {
		if (strstr (sections[i].name, "__TEXT.__thread_starts")) {
			starts_offset = sections[i].offset;
			starts_size = sections[i].size;
			break;
		}
	}

	R_FREE (sections);

	ut64 kernel_base = 0;

	struct MACH0_(segment_command) *seg;
	int nsegs = R_MIN (mach0->nsegs, 128);
	for (i = 0; i < nsegs; i++) {
		char segname[17];
		seg = &mach0->segs[i];
		r_str_ncpy (segname, seg->segname, 17);
		if (!strncmp (segname, "__TEXT", 6) && segname[6] == '\0') {
			kernel_base = seg->vmaddr;
			break;
		}
	}

	if (starts_offset == 0 || starts_size == 0 || kernel_base == 0) {
		return NULL;
	}

	int n_starts = starts_size / 4;
	if (n_starts <= 1) {
		return NULL;
	}
	rebase_ranges = R_NEWS0 (RFileRange, n_starts - 1);
	if (rebase_ranges == NULL) {
		return NULL;
	}

	ut64 multiplier = 4;
	for (i = 0; i != n_starts; i++) {
		ut8 bytes[4];
		if (r_buf_read_at (cache_buf, starts_offset + i * 4, bytes, 4) < 4) {
			goto beach;
		}

		if (i == 0) {
			multiplier += 4 * (r_read_le32 (bytes) & 1);
			continue;
		}

		rebase_ranges[i - 1].offset = r_read_le32 (bytes);
		rebase_ranges[i - 1].size = UT64_MAX;
	}

	RRebaseInfo *rebase_info = R_NEW0 (RRebaseInfo);
	if (rebase_info == NULL) {
		goto beach;
	}
	rebase_info->ranges = rebase_ranges;
	rebase_info->n_ranges = n_starts - 1;
	rebase_info->multiplier = multiplier;
	rebase_info->kernel_base = kernel_base;

	return rebase_info;

beach:

	R_FREE (rebase_ranges);
	return NULL;
}