static void wasm_custom_name_free(RBinWasmCustomNameEntry *cust) {
	if (cust) {
		switch (cust->type) {
		case R_BIN_WASM_NAMETYPE_Module:
			R_FREE (cust->mod_name);
			break;
		case R_BIN_WASM_NAMETYPE_Function:
			if (cust->func) {
				r_id_storage_free (cust->func->names);
				R_FREE (cust->func);
			}
			break;
		case R_BIN_WASM_NAMETYPE_Local:
			wasm_custom_local_names_free (cust->local);
			break;
		case R_BIN_WASM_NAMETYPE_None:
			break;
		default:
			eprintf ("Unkown type: 0x%x\n", cust->type);
			r_warn_if_reached ();
		}
		R_FREE (cust);
	}
}