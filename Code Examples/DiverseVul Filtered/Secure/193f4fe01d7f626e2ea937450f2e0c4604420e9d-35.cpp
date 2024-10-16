R_API RList *r_bin_file_get_symbols(RBinFile *bf) {
	r_return_val_if_fail (bf, NULL);
	RBinObject *o = bf->o;
	return o? o->symbols: NULL;
}