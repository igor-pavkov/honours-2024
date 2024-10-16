static RList *carve_kexts(RKernelCacheObj *obj, RBinFile *bf) {
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (obj->mach0))) {
		return NULL;
	}

	ut64 pa2va_exec = 0;
	ut64 pa2va_data = 0;
	ut64 kmod_start = 0, kmod_end = 0;
	ut64 kmod_info = 0, kmod_info_end = 0;
	int incomplete = 4;
	RKmodInfo *all_infos = NULL;

	int i = 0;
	for (; !sections[i].last && incomplete > 0; i++) {
		if (strstr (sections[i].name, "__TEXT_EXEC.__text")) {
			pa2va_exec = sections[i].addr - sections[i].offset;
			incomplete--;
		}
		if (strstr (sections[i].name, "__DATA.__data")) {
			pa2va_data = sections[i].addr - sections[i].offset;
			incomplete--;
		}
		if (strstr (sections[i].name, "__PRELINK_INFO.__kmod_start")) {
			kmod_start = sections[i].offset;
			kmod_end = kmod_start + sections[i].size;
			incomplete--;
		}
		if (strstr (sections[i].name, "__PRELINK_INFO.__kmod_info")) {
			kmod_info = sections[i].offset;
			kmod_info_end = kmod_info + sections[i].size;
			incomplete--;
		}
	}

	R_FREE (sections);

	if (incomplete) {
		return NULL;
	}

	RList *kexts = r_list_newf ((RListFree) &r_kext_free);
	if (!kexts) {
		return NULL;
	}

	int n_kmod_info = (kmod_info_end - kmod_info) / 8;
	if (n_kmod_info == 0) {
		goto beach;
	}

	all_infos = R_NEWS0 (RKmodInfo, n_kmod_info);
	if (!all_infos) {
		goto beach;
	}

	ut8 bytes[8];
	int j = 0;
	for (; j < n_kmod_info; j++) {
		ut64 entry_offset = j * 8 + kmod_info;

		if (r_buf_read_at (obj->cache_buf, entry_offset, bytes, 8) < 8) {
			goto beach;
		}

		ut64 kmod_info_paddr = K_RPTR (bytes) - pa2va_data;

		ut64 field_name = kmod_info_paddr + 0x10;
		ut64 field_start = kmod_info_paddr + 0xb4;

		if (r_buf_read_at (obj->cache_buf, field_start, bytes, 8) < 8) {
			goto beach;
		}

		all_infos[j].start = K_RPTR (bytes);

		if (r_buf_read_at (obj->cache_buf, field_name, (ut8 *) all_infos[j].name, 0x40) < 0x40) {
			goto beach;
		}

		all_infos[j].name[0x40] = 0;
	}

	ut64 cursor = kmod_start;
	for(; cursor < kmod_end; cursor += 8) {
		ut8 bytes[8];
		if (r_buf_read_at (obj->cache_buf, cursor, bytes, 8) < 8) {
			goto beach;
		}

		RKext *kext = R_NEW0 (RKext);
		if (!kext) {
			goto beach;
		}

		kext->vaddr = K_RPTR (bytes);
		kext->range.offset = kext->vaddr - pa2va_exec;

		kext->mach0 = create_kext_mach0 (obj, kext, bf);
		if (!kext->mach0) {
			r_kext_free (kext);
			continue;
		}

		r_kext_fill_text_range (kext);
		kext->vaddr = K_PPTR (kext->vaddr);
		kext->pa2va_exec = pa2va_exec;
		kext->pa2va_data = pa2va_data;

		ut64 text_start = kext->vaddr;
		ut64 text_end = text_start + kext->text_range.size;

		if (text_start == text_end) {
			r_kext_free (kext);
			continue;
		}

		for (j = 0; j < n_kmod_info; j++) {
			if (text_start > all_infos[j].start || all_infos[j].start >= text_end) {
				continue;
			}

			kext->name = strdup (all_infos[j].name);
			kext->own_name = true;
			break;
		}

		if (!kext->name) {
			r_kext_free (kext);
			continue;
		}

		r_list_push (kexts, kext);
	}

	R_FREE (all_infos);
	return kexts;

beach:
	r_list_free (kexts);
	R_FREE (all_infos);
	return NULL;
}