static RList *kexts_from_load_commands(RKernelCacheObj *obj, RBinFile *bf) {
	RList *kexts = r_list_newf ((RListFree) &r_kext_free);
	if (!kexts) {
		return NULL;
	}

	ut32 i, ncmds = r_buf_read_le32_at (obj->cache_buf, 16);
	ut64 length = r_buf_size (obj->cache_buf);

	ut32 cursor = sizeof (struct MACH0_(mach_header));
	for (i = 0; i < ncmds && cursor < length; i++) {
		ut32 cmdtype = r_buf_read_le32_at (obj->cache_buf, cursor);
		ut32 cmdsize = r_buf_read_le32_at (obj->cache_buf, cursor + 4);
		if (!cmdsize || cmdsize + cursor < cursor) {
			break;
		}
		if (cmdtype != LC_KEXT) {
			cursor += cmdsize;
			continue;
		}

		ut64 vaddr = r_buf_read_le64_at (obj->cache_buf, cursor + 8);
		ut64 paddr = r_buf_read_le64_at (obj->cache_buf, cursor + 16);
		st32 padded_name_length = (st32)cmdsize - 32;
		if (padded_name_length <= 0 || cmdsize - 32 + cursor >= length) {
			cursor += cmdsize;
			continue;
		}

		char *padded_name = calloc (1, padded_name_length);
		if (!padded_name) {
			goto beach;
		}
		if (r_buf_read_at (obj->cache_buf, cursor + 32, (ut8 *)padded_name, padded_name_length)
				!= padded_name_length) {
			free (padded_name);
			goto early;
		}

		RKext *kext = R_NEW0 (RKext);
		if (!kext) {
			free (padded_name);
			goto beach;
		}

		kext->vaddr = vaddr;
		kext->range.offset = paddr;

		kext->mach0 = create_kext_shared_mach0 (obj, kext, bf);
		if (!kext->mach0) {
			free (padded_name);
			r_kext_free (kext);
			cursor += cmdsize;
			continue;
		}

		r_kext_fill_text_range (kext);
		kext->vaddr = K_PPTR (kext->vaddr);
		kext->pa2va_exec = obj->pa2va_exec;
		kext->pa2va_data = obj->pa2va_data;
		kext->name = strdup (padded_name);
		kext->own_name = true;
		free (padded_name);
		r_list_push (kexts, kext);

		cursor += cmdsize;
	}
early:
	return kexts;
beach:
	r_list_free (kexts);
	return NULL;
}