R_API void r_bin_java_external_printf(PrintfCallback cb) {
	Eprintf = cb ? cb : (PrintfCallback) printf;
}