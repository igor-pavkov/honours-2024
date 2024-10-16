const char *r_bin_wasm_valuetype_to_string (r_bin_wasm_value_type_t type) {
	switch (type) {
	case R_BIN_WASM_VALUETYPE_i32:
		return "i32";
	case R_BIN_WASM_VALUETYPE_i64:
		return "i62";
	case R_BIN_WASM_VALUETYPE_f32:
		return "f32";
	case R_BIN_WASM_VALUETYPE_f64:
		return "f64";
	case R_BIN_WASM_VALUETYPE_REFTYPE:
		return "ANYFUNC";
	case R_BIN_WASM_VALUETYPE_FUNC:
		return "FUNC";
	default:
		return "<?>";
	}
}