static void ensure_kexts_initialized(RKernelCacheObj *obj, RBinFile *bf) {
	if (obj->kexts_initialized) {
		return;
	}
	obj->kexts_initialized = true;

	RList *kexts = NULL;

	if (obj->prelink_info) {
		kexts = filter_kexts (obj, bf);
	}

	if (kexts && !r_list_length (kexts)) {
		r_list_free (kexts);
		kexts = NULL;
	}

	if (!kexts) {
		kexts = kexts_from_load_commands (obj, bf);
	}

	if (kexts && !r_list_length (kexts)) {
		r_list_free (kexts);
		kexts = NULL;
	}

	if (!kexts) {
		kexts = carve_kexts (obj, bf);
	}

	obj->kexts = r_kext_index_new (kexts);

	if (kexts) {
		kexts->free = NULL;
		r_list_free (kexts);
	}
}