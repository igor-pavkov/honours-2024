R_API int r_bin_load_languages(RBinFile *binfile) {
	if (r_bin_lang_rust (binfile)) {
		return R_BIN_NM_RUST;
	}
	if (r_bin_lang_swift (binfile)) {
		return R_BIN_NM_SWIFT;
	}
	if (r_bin_lang_objc (binfile)) {
		return R_BIN_NM_OBJC;
	}
	if (r_bin_lang_cxx (binfile)) {
		return R_BIN_NM_CXX;
	}
	if (r_bin_lang_dlang (binfile)) {
		return R_BIN_NM_DLANG;
	}
	if (r_bin_lang_msvc (binfile)) {
		return R_BIN_NM_MSVC;
	}
	return R_BIN_NM_NONE;
}