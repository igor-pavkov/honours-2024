static RList *symbols(RBinFile *bf) {
	RList *ret = r_list_newf (free);
	if (!ret) {
		return NULL;
	}

	RKernelCacheObj *obj = (RKernelCacheObj*) bf->o->bin_obj;

	symbols_from_mach0 (ret, obj->mach0, bf, 0, 0);

	HtPP *kernel_syms_by_addr = sdb_ht_new ();
	if (!kernel_syms_by_addr) {
		r_list_free (ret);
		return NULL;
	}

	RListIter *iter;
	RBinSymbol *sym;
	ut64 enosys_addr = 0;
	r_list_foreach (ret, iter, sym) {
		r_strf_var (key, 64, "%"PFMT64x, sym->vaddr);
		sdb_ht_insert (kernel_syms_by_addr, key, sym->dname ? sym->dname : sym->name);
		if (!enosys_addr && strstr (sym->name, "enosys")) {
			enosys_addr = sym->vaddr;
		}
	}

	RList *syscalls = resolve_syscalls (obj, enosys_addr);
	if (syscalls) {
		r_list_foreach (syscalls, iter, sym) {
			r_strf_var (key, 32, "%"PFMT64x, sym->vaddr);
			sdb_ht_insert (kernel_syms_by_addr, key, sym->name);
			r_list_append (ret, sym);
		}
		syscalls->free = NULL;
		r_list_free (syscalls);
	}

	RList *subsystem = resolve_mig_subsystem (obj);
	if (subsystem) {
		r_list_foreach (subsystem, iter, sym) {
			r_strf_var (key, 64, "%"PFMT64x, sym->vaddr);
			sdb_ht_insert (kernel_syms_by_addr, key, sym->name);
			r_list_append (ret, sym);
		}
		subsystem->free = NULL;
		r_list_free (subsystem);
	}

	ensure_kexts_initialized (obj, bf);

	RKext *kext;
	int kiter;
	ut64 *inits = NULL;
	ut64 *terms = NULL;
	r_kext_index_foreach (obj->kexts, kiter, kext) {
		ut8 magicbytes[4];
		r_buf_read_at (obj->cache_buf, kext->range.offset, magicbytes, 4);
		int magic = r_read_le32 (magicbytes);
		switch (magic) {
		case MH_MAGIC_64:
			symbols_from_mach0 (ret, kext->mach0, bf, kext->range.offset, r_list_length (ret));
			symbols_from_stubs (ret, kernel_syms_by_addr, obj, bf, kext, r_list_length (ret));
			process_constructors (obj, kext->mach0, ret, kext->range.offset, false, R_K_CONSTRUCTOR_TO_SYMBOL, kext_short_name (kext));
			process_kmod_init_term (obj, kext, ret, &inits, &terms);

			break;
		default:
			eprintf ("Unknown sub-bin\n");
			break;
		}
	}

	R_FREE (inits);
	R_FREE (terms);

	sdb_ht_free (kernel_syms_by_addr);

	return ret;
}