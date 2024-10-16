static RList *resolve_mig_subsystem(RKernelCacheObj *obj) {
	struct section_t *sections = NULL;
	if (!(sections = MACH0_(get_sections) (obj->mach0))) {
		return NULL;
	}

	HtPP *mig_hash = NULL;
	RList *subsystem = NULL;
	ut8 *data_const = NULL;
	ut64 data_const_offset = 0, data_const_size = 0, data_const_vaddr = 0;
	ut64 text_exec_offset = 0, text_exec_size = 0, text_exec_vaddr = 0;
	int incomplete = 2;
	int i = 0;
	for (; !sections[i].last && incomplete > 0; i++) {
		if (strstr (sections[i].name, "__DATA_CONST.__const")) {
			data_const_offset = sections[i].offset;
			data_const_size = sections[i].size;
			data_const_vaddr = K_PPTR (sections[i].addr);
			incomplete--;
		}
		if (strstr (sections[i].name, "__TEXT_EXEC.__text")) {
			text_exec_offset = sections[i].offset;
			text_exec_size = sections[i].size;
			text_exec_vaddr = K_PPTR (sections[i].addr);
			incomplete--;
		}
	}

	if (!data_const_offset || !data_const_size || !data_const_vaddr ||
		!text_exec_offset || !text_exec_size || !text_exec_vaddr) {
		goto beach;
	}

	data_const = malloc (data_const_size);
	if (!data_const) {
		goto beach;
	}
	if (r_buf_read_at (obj->cache_buf, data_const_offset, data_const, data_const_size) < data_const_size) {
		goto beach;
	}

	subsystem = r_list_newf (r_bin_symbol_free);
	if (!subsystem) {
		goto beach;
	}

	mig_hash = mig_hash_new ();
	if (!mig_hash) {
		goto beach;
	}

	ut8 *cursor = data_const;
	ut8 *end = data_const + data_const_size;
	while (cursor < end) {
		ut64 subs_p = K_PPTR (r_read_le64 (cursor));
		if (subs_p < text_exec_vaddr || subs_p >= text_exec_vaddr + text_exec_size) {
			cursor += 8;
			continue;
		}
		ut32 subs_min_idx = r_read_le32 (cursor + 8);
		ut32 subs_max_idx = r_read_le32 (cursor + 12);
		if (subs_min_idx >= subs_max_idx || (subs_max_idx - subs_min_idx) > K_MIG_MAX_ROUTINES) {
			cursor += 16;
			continue;
		}

		ut32 n_routines = (subs_max_idx - subs_min_idx);
		ut64 *routines = (ut64 *) calloc (n_routines, sizeof (ut64));
		if (!routines) {
			goto beach;
		}
		ut8 *array_cursor = cursor + K_MIG_SUBSYSTEM_SIZE;
		ut8 *end_array = array_cursor + n_routines * K_MIG_ROUTINE_SIZE;
		bool is_consistent = true;
		int idx = 0;
		while (array_cursor < end_array) {
			ut64 should_be_null = r_read_le64 (array_cursor);
			if (should_be_null != 0) {
				is_consistent = false;
				break;
			}

			ut64 routine_p = K_PPTR (r_read_le64 (array_cursor + 8));
			if (routine_p != 0 && (routine_p < text_exec_vaddr || routine_p >= text_exec_vaddr + text_exec_size)) {
				is_consistent = false;
				break;
			}

			routines[idx++] = routine_p;
			array_cursor += K_MIG_ROUTINE_SIZE;
		}

		if (is_consistent) {
			for (idx = 0; idx < n_routines; idx++) {
				ut64 routine_p = routines[idx];
				if (!routine_p) {
					continue;
				}

				RBinSymbol *sym = R_NEW0 (RBinSymbol);
				if (!sym) {
					R_FREE (routines);
					goto beach;
				}

				int num = idx + subs_min_idx;
				bool found = false;
				r_strf_var (key, 32, "%d", num);
				const char *name = sdb_ht_find (mig_hash, key, &found);
				if (found && name && *name) {
					sym->name = r_str_newf ("mig.%d.%s", num, name);
				} else {
					sym->name = r_str_newf ("mig.%d", num);
				}

				sym->vaddr = routine_p;
				sym->paddr = sym->vaddr - text_exec_vaddr + text_exec_offset;
				sym->size = 0;
				sym->forwarder = "NONE";
				sym->bind = "GLOBAL";
				sym->type = "OBJECT";
				r_list_append (subsystem, sym);
			}

			cursor += K_MIG_SUBSYSTEM_SIZE + n_routines * K_MIG_ROUTINE_SIZE;
		} else {
			cursor += 8;
		}

		R_FREE (routines);
	}

	sdb_ht_free (mig_hash);
	R_FREE (data_const);
	R_FREE (sections);
	return subsystem;

beach:
	if (subsystem) {
		r_list_free (subsystem);
	}
	if (mig_hash) {
		sdb_ht_free (mig_hash);
	}
	R_FREE (data_const);
	R_FREE (sections);
	return NULL;
}