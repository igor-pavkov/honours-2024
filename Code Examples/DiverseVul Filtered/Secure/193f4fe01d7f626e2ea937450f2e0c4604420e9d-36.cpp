static RBinSymbol *__getMethod(RBinFile *bf, const char *klass, const char *method) {
	r_return_val_if_fail (bf && bf->o && bf->o->methods_ht && klass && method, NULL);
	r_strf_var (name, 128, "%s::%s", klass, method);
	return ht_pp_find (bf->o->methods_ht, name, NULL);
}