static struct MACH0_(obj_t) *create_kext_mach0(RKernelCacheObj *obj, RKext *kext, RBinFile *bf) {
	RBuffer *buf = r_buf_new_slice (obj->cache_buf, kext->range.offset, r_buf_size (obj->cache_buf) - kext->range.offset);
	struct MACH0_(opts_t) opts;
	MACH0_(opts_set_default) (&opts, bf);
	opts.verbose = true;
	opts.header_at = 0;
	struct MACH0_(obj_t) *mach0 = MACH0_(new_buf) (buf, &opts);
	r_buf_free (buf);
	return mach0;
}