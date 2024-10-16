static struct MACH0_(obj_t) *create_kext_shared_mach0(RKernelCacheObj *obj, RKext *kext, RBinFile *bf) {
	RBuffer *buf = r_buf_ref (obj->cache_buf);
	struct MACH0_(opts_t) opts;
	MACH0_(opts_set_default) (&opts, bf);
	opts.verbose = false;
	opts.header_at = kext->range.offset;
	struct MACH0_(obj_t) *mach0 = MACH0_(new_buf) (buf, &opts);
	r_buf_free (buf);
	return mach0;
}