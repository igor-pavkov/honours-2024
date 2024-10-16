static RPVector *parse_sub_section_vec(RBinWasmObj *bin, RBinWasmSection *sec) {
	RPVector **cache = NULL;
	RPVectorFree pfree = (RPVectorFree)free;
	ParseEntryFcn parser;
	switch (sec->id) {
	case R_BIN_WASM_SECTION_TYPE:
		parser = (ParseEntryFcn)parse_type_entry;
		pfree = (RPVectorFree)free_type_entry;
		cache = &bin->g_types;
		break;
	case R_BIN_WASM_SECTION_IMPORT:
		parser = (ParseEntryFcn)parse_import_entry;
		pfree = (RPVectorFree)free_import_entry;
		cache = &bin->g_imports;
		break;
	case R_BIN_WASM_SECTION_FUNCTION:
		parser = (ParseEntryFcn)parse_function_entry;
		cache = &bin->g_funcs;
		break;
	case R_BIN_WASM_SECTION_TABLE:
		parser = (ParseEntryFcn)parse_table_entry;
		cache = &bin->g_tables;
		break;
	case R_BIN_WASM_SECTION_MEMORY:
		parser = (ParseEntryFcn)parse_memory_entry;
		cache = &bin->g_memories;
		break;
	case R_BIN_WASM_SECTION_GLOBAL:
		parser = (ParseEntryFcn)parse_global_entry;
		cache = &bin->g_globals;
		break;
	case R_BIN_WASM_SECTION_EXPORT:
		parser = (ParseEntryFcn)parse_export_entry;
		pfree = (RPVectorFree)free_export_entry;
		cache = &bin->g_exports;
		break;
	case R_BIN_WASM_SECTION_ELEMENT:
		parser = (ParseEntryFcn)parse_element_entry;
		cache = &bin->g_elements;
		break;
	case R_BIN_WASM_SECTION_CODE:
		parser = (ParseEntryFcn)parse_code_entry;
		pfree = (RPVectorFree)free_code_entry;
		cache = &bin->g_codes;
		break;
	case R_BIN_WASM_SECTION_DATA:
		parser = (ParseEntryFcn)parse_data_entry;
		cache = &bin->g_datas;
		break;
	default:
		return NULL;
	}

	RBuffer *buf = bin->buf;
	ut64 offset = sec->payload_data;
	ut64 len = sec->payload_len;
	ut64 bound = offset + len - 1;

	if (bound >= r_buf_size (buf)) {
		r_warn_if_reached (); // section parsing should prevent this
		eprintf ("[wasm] End of %s section data is beyond file end\n", sec->name);
		return NULL;
	}
	if (r_buf_seek (buf, offset, R_BUF_SET) != offset) {
		return NULL;
	}

	*cache = parse_vec (bin, bound, parser, pfree);
	return *cache;
}