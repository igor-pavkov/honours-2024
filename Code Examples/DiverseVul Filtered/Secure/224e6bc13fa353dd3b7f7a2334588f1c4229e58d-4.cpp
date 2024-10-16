static int java_reset_counter (RAnal *anal, ut64 start_addr ) {
	IFDBG eprintf ("Setting the new METHOD_START to 0x%08"PFMT64x" was 0x%08"PFMT64x"\n", start_addr, METHOD_START);
	METHOD_START = start_addr;
	r_java_new_method ();
	return true;
}