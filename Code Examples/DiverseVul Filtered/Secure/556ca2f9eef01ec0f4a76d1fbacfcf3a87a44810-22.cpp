RZ_API objc_cache_opt_info *rz_dyldcache_get_objc_opt_info(RzBinFile *bf, RzDyldCache *cache) {
	objc_cache_opt_info *result = NULL;
	RzListIter *iter;
	RzDyldBinImage *bin;
	rz_list_foreach (cache->bins, iter, bin) {
		if (strcmp(bin->file, "lib/libobjc.A.dylib")) {
			continue;
		}

		struct MACH0_(opts_t) opts = { 0 };
		opts.verbose = bf->rbin->verbose;
		opts.header_at = bin->header_at;

		struct MACH0_(obj_t) *mach0 = MACH0_(new_buf)(cache->buf, &opts);
		if (!mach0) {
			goto beach;
		}

		struct section_t *sections = NULL;
		if (!(sections = MACH0_(get_sections)(mach0))) {
			MACH0_(mach0_free)
			(mach0);
			goto beach;
		}

		int i;
		ut64 scoffs_offset = 0;
		ut64 scoffs_size = 0;
		ut64 slide = rz_dyldcache_get_slide(cache);
		for (i = 0; !sections[i].last; i++) {
			if (sections[i].size == 0) {
				continue;
			}
			if (strstr(sections[i].name, "__objc_scoffs")) {
				scoffs_offset = va2pa(sections[i].addr, cache->n_maps, cache->maps, cache->buf, slide, NULL, NULL);
				scoffs_size = sections[i].size;
				break;
			}
		}

		MACH0_(mach0_free)
		(mach0);
		RZ_FREE(sections);

		if (!scoffs_offset || scoffs_size < 40) {
			break;
		}
		ut64 check;
		if (!rz_buf_read_le64_at(cache->buf, scoffs_offset, &check) || check != 2) {
			break;
		}
		ut64 sel_string_base;
		if (!rz_buf_read_le64_at(cache->buf, scoffs_offset + 8, &sel_string_base)) {
			break;
		}
		ut64 sel_string_end;
		if (!rz_buf_read_le64_at(cache->buf, scoffs_offset + 16, &sel_string_end) || sel_string_end == sel_string_base) {
			break;
		}
		result = RZ_NEW0(objc_cache_opt_info);
		if (!result) {
			break;
		}
		result->sel_string_base = sel_string_base;
	}
beach:
	return result;
}