static int check_addr_less_start (RBinJavaField *method, ut64 addr) {
	ut64 start = r_bin_java_get_method_code_offset (method);
	return (addr < start);
}