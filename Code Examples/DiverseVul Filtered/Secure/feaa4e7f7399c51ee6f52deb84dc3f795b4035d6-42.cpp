static void r_rebase_info_populate(RRebaseInfo *info, RKernelCacheObj *obj) {
	struct section_t *sections = NULL;
	int i = 0;

	if (obj->rebase_info_populated) {
		return;
	}
	obj->rebase_info_populated = true;

	for (; i < info->n_ranges; i++) {
		if (info->ranges[i].size != UT64_MAX) {
			goto cleanup;
		} else if (sections == NULL) {
			if (!(sections = MACH0_(get_sections) (obj->mach0))) {
				return;
			}
		}
		info->ranges[i].offset = r_rebase_offset_to_paddr (obj, sections, info->ranges[i].offset);
		ut64 end = iterate_rebase_list (obj->cache_buf, info->multiplier, info->ranges[i].offset, NULL, NULL);
		if (end != UT64_MAX) {
			info->ranges[i].size = end - info->ranges[i].offset + 8;
		} else {
			info->ranges[i].size = 0;
		}
	}

cleanup:
	R_FREE (sections);
}