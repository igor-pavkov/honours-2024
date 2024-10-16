static ut64 baddr(RBinFile *bf) {
	if (!bf || !bf->o || !bf->o->bin_obj) {
		return 8LL;
	}

	RKernelCacheObj *obj = (RKernelCacheObj*) bf->o->bin_obj;
	return MACH0_(get_baddr)(obj->mach0);
}