void wasm_obj_free(RBinWasmObj *bin) {
	if (bin) {
		r_buf_free (bin->buf);
		r_list_free (bin->g_sections);
		r_pvector_free (bin->g_types);
		r_pvector_free (bin->g_imports);
		r_pvector_free (bin->g_funcs);
		r_pvector_free (bin->g_tables);
		r_pvector_free (bin->g_memories);
		r_pvector_free (bin->g_globals);
		r_pvector_free (bin->g_exports);
		r_pvector_free (bin->g_elements);
		r_pvector_free (bin->g_codes);
		r_pvector_free (bin->g_datas);
		r_list_free (bin->g_names);
		free (bin);
	}
}