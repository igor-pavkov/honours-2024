static int check_addr_less_end (RBinJavaField *method, ut64 addr) {
	ut64 end = r_bin_java_get_method_code_size (method);
	return (addr < end);
}