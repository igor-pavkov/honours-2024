static int java_new_method (ut64 method_start) {
	METHOD_START = method_start;
	// reset the current bytes consumed counter
	r_java_new_method ();
	return 0;
}