static RBinClass *__getClass(RBinFile *bf, const char *name) {
	r_return_val_if_fail (bf && bf->o && bf->o->classes_ht && name, NULL);
	return ht_pp_find (bf->o->classes_ht, name, NULL);
}