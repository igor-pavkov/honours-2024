void r_bin_wasm_destroy(RBinFile *bf) {
	if (bf && bf->o) {
		wasm_obj_free (bf->o->bin_obj);
		bf->o->bin_obj = NULL;
	}
}