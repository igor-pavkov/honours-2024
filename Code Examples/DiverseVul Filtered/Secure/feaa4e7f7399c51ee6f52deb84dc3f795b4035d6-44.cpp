static void symbols_from_stubs(RList *ret, HtPP *kernel_syms_by_addr, RKernelCacheObj *obj, RBinFile *bf, RKext *kext, int ordinal) {
	RStubsInfo *stubs_info = get_stubs_info(kext->mach0, kext->range.offset, obj);
	if (!stubs_info) {
		return;
	}
	ut64 stubs_cursor = stubs_info->stubs.offset;
	ut64 stubs_end = stubs_cursor + stubs_info->stubs.size;

	for (; stubs_cursor < stubs_end; stubs_cursor += 12) {
		ut8 arm64_code[8];
		if (r_buf_read_at (obj->cache_buf, stubs_cursor, arm64_code, 8) < 8) {
			break;
		}

		ut64 vaddr = stubs_cursor + obj->pa2va_exec;
		ut64 addr_in_got = extract_addr_from_code (arm64_code, vaddr);

		bool found = false;
		int level = 3;

		ut64 target_addr = UT64_MAX;

		while (!found && level-- > 0) {
			ut64 offset_in_got = addr_in_got - obj->pa2va_exec;
			ut64 addr;
			if (r_buf_read_at (obj->cache_buf, offset_in_got, (ut8*) &addr, 8) < 8) {
				break;
			}

			if (level == 2) {
				target_addr = addr;
			}

			r_strf_var (key, 32, "%"PFMT64x, addr);
			const char *name = sdb_ht_find (kernel_syms_by_addr, key, &found);

			if (found) {
				RBinSymbol *sym = R_NEW0 (RBinSymbol);
				if (!sym) {
					break;
				}
				sym->name = r_str_newf ("stub.%s", name);
				sym->vaddr = vaddr;
				sym->paddr = stubs_cursor;
				sym->size = 12;
				sym->forwarder = "NONE";
				sym->bind = "LOCAL";
				sym->type = "FUNC";
				sym->ordinal = ordinal ++;
				r_list_append (ret, sym);
				break;
			}

			addr_in_got = addr;
		}

		if (found || target_addr == UT64_MAX) {
			continue;
		}

		ensure_kexts_initialized (obj, bf);
		RKext *remote_kext = r_kext_index_vget (obj->kexts, target_addr);
		if (!remote_kext) {
			continue;
		}

		RBinSymbol *remote_sym = R_NEW0 (RBinSymbol);
		if (!remote_sym) {
			break;
		}

		remote_sym->name = r_str_newf ("exp.%s.0x%"PFMT64x, kext_short_name (remote_kext), target_addr);
		remote_sym->vaddr = target_addr;
		remote_sym->paddr = target_addr - obj->pa2va_exec;
		remote_sym->size = 0;
		remote_sym->forwarder = "NONE";
		remote_sym->bind = "GLOBAL";
		remote_sym->type = "FUNC";
		remote_sym->ordinal = ordinal ++;
		r_list_append (ret, remote_sym);

		RBinSymbol *local_sym = R_NEW0 (RBinSymbol);
		if (!local_sym) {
			break;
		}

		local_sym->name = r_str_newf ("stub.%s.0x%"PFMT64x, kext_short_name (remote_kext), target_addr);
		local_sym->vaddr = vaddr;
		local_sym->paddr = stubs_cursor;
		local_sym->size = 12;
		local_sym->forwarder = "NONE";
		local_sym->bind = "GLOBAL";
		local_sym->type = "FUNC";
		local_sym->ordinal = ordinal ++;
		r_list_append (ret, local_sym);
	}

	R_FREE (stubs_info);
}