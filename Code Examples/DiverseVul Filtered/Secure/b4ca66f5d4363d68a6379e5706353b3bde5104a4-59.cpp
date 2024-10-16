RList *r_bin_wasm_get_custom_names(RBinWasmObj *bin) {
	RList *customs = NULL;

	r_return_val_if_fail (bin && bin->g_sections, NULL);

	if (bin->g_names) {
		return bin->g_names;
	}
	if (!(customs = r_bin_wasm_get_sections_by_id (bin->g_sections, R_BIN_WASM_SECTION_CUSTOM))) {
		return r_list_new ();
	}
	// support for multiple "name" sections against spec
	RBinWasmSection *cust = (RBinWasmSection *)r_list_first (customs);
	if (!cust || !cust->name) {
		r_list_free (customs);
		return r_list_new ();
	}
	if (strcmp (cust->name, "name")) {
		r_list_free (customs);
		return r_list_new ();
	}
	bin->g_names = r_bin_wasm_get_custom_name_entries (bin, cust);
	r_list_free (customs);
	return bin->g_names;
}