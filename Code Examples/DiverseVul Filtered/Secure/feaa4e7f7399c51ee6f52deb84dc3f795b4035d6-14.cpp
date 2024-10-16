static void destroy(RBinFile *bf) {
	r_kernel_cache_free ((RKernelCacheObj*) bf->o->bin_obj);
}