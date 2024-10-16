static RBinWasmCustomNameEntry *parse_custom_name_entry(RBinWasmObj *bin, ut64 bound) {
	RBuffer *b = bin->buf;
	RBinWasmCustomNameEntry *cust = R_NEW0 (RBinWasmCustomNameEntry);
	if (!cust) {
		return NULL;
	}
	cust->type = R_BIN_WASM_NAMETYPE_None;

	size_t start = r_buf_tell (b);
	if (!consume_u7_r (b, bound, &cust->type)) {
		goto beach;
	};

	if (!consume_u32_r (b, bound, &cust->size)) {
		goto beach;
	};

	switch (cust->type) {
	case R_BIN_WASM_NAMETYPE_Module:
		if (!consume_encoded_name_new (b, bound, NULL, &cust->mod_name)) {
			goto beach;
		}
		break;
	case R_BIN_WASM_NAMETYPE_Function:
		cust->func = R_NEW0 (RBinWasmCustomNameFunctionNames);
		if (!cust->func) {
			goto beach;
		}
		cust->func->names = r_id_storage_new (0, UT32_MAX);
		if (!cust->func->names) {
			goto beach;
		}

		if (!parse_namemap (b, bound, cust->func->names, &cust->func->count)) {
			goto beach;
		}
		break;
	case R_BIN_WASM_NAMETYPE_Local:
		cust->local = parse_custom_names_local (b, bound);
		if (!cust->local) {
			goto beach;
		}
		break;
	default:
		R_LOG_WARN ("[wasm] Halting custom name section parsing at unknown type 0x%x offset 0x%" PFMTSZx "\n", cust->type, start);
		cust->type = R_BIN_WASM_NAMETYPE_None;
		goto beach;
	}

	return cust;
beach:
	wasm_custom_name_free (cust);
	return NULL;
}