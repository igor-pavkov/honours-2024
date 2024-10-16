static int check_addr_in_code (RBinJavaField *method, ut64 addr) {
	return !check_addr_less_start (method, addr) && \
		check_addr_less_end ( method, addr);
}