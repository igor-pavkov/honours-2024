static int java_cmd_ext(RAnal *anal, const char* input) {
	RBinJavaObj *obj = (RBinJavaObj *) get_java_bin_obj (anal);

	if (!obj) {
		eprintf ("Execute \"af\" to set the current bin, and this will bind the current bin\n");
		return -1;
	}
	switch (*input) {
	case 'c':
		// reset bytes counter for case operations
		r_java_new_method ();
		break;
	case 'u':
		switch (*(input+1)) {
			case 't': {java_update_anal_types (anal, obj); return true;}
			default: break;
		}
		break;
	case 's':
		switch (*(input+1)) {
			//case 'e': return java_resolve_cp_idx_b64 (anal, input+2);
			default: break;
		}
		break;

	default: eprintf("Command not supported"); break;
	}
	return 0;
}