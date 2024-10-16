static void destroy(RBinFile *bf) {
	r_coresym_cache_element_free (bf->o->bin_obj);
}