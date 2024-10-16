static void r_kernel_cache_free(RKernelCacheObj *obj) {
	if (!obj) {
		return;
	}

	if (obj->mach0) {
		MACH0_(mach0_free) (obj->mach0);
		obj->mach0 = NULL;
		obj->cache_buf = NULL;
	}

	if (obj->cache_buf) {
		r_buf_free (obj->cache_buf);
		obj->cache_buf = NULL;
	}

	if (obj->prelink_info) {
		r_cf_value_dict_free (obj->prelink_info);
		obj->prelink_info = NULL;
	}

	if (obj->kexts) {
		r_kext_index_free (obj->kexts);
		obj->kexts = NULL;
	}

	if (obj->rebase_info) {
		r_rebase_info_free (obj->rebase_info);
		obj->rebase_info = NULL;
	}

	R_FREE (obj);
}