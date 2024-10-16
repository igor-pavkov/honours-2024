static int java_analyze_fns( RAnal *anal, ut64 start, ut64 end, int reftype, int depth) {
	//anal->iob.read_at (anal->iob.io, op.jump, bbuf, sizeof (bbuf));
	RBinJavaObj *bin = NULL;// = get_java_bin_obj (anal);
	RBinJavaField *method = NULL;
	RListIter *methods_iter, *bin_obs_iter;
	RList * bin_objs_list = get_java_bin_obj_list (anal);

	ut8 analyze_all = 0;
	//RAnalRef *ref = NULL;
	int result = R_ANAL_RET_ERROR;

	if (end == UT64_MAX) {
		analyze_all = 1;
	}
	if (!bin_objs_list || r_list_empty (bin_objs_list)) {
		r_list_free (bin_objs_list);
		return java_analyze_fns_from_buffer (anal, start, end, reftype, depth);
	}
	r_list_foreach (bin_objs_list, bin_obs_iter, bin) {
		// loop over all bin object that are loaded
		java_update_anal_types (anal, bin);
		RList *methods_list = (RList *) r_bin_java_get_methods_list (bin);
		ut64 loadaddr = bin->loadaddr;
		// loop over all methods in the binary object and analyse
		// the functions
		r_list_foreach (methods_list, methods_iter, method) {
			if ((method && analyze_all) ||
			    (check_addr_less_start (method, end) ||
			     check_addr_in_code (method, end))) {
				RAnalFunction *fcn = r_anal_fcn_new ();
				fcn->cc = r_str_const (r_anal_cc_default (anal));
				java_set_function_prototype (anal, fcn, method);
				result = analyze_from_code_attr (anal, fcn, method, loadaddr);
				if (result == R_ANAL_RET_ERROR) {
					//eprintf ("Failed to parse java fn: %s @ 0x%04"PFMT64x"\n", fcn->name, fcn->addr);
					return result;
					// XXX - TO Stop or not to Stop ??
				}
				//r_listrange_add (anal->fcnstore, fcn);
				r_anal_fcn_update_tinyrange_bbs (fcn);
				r_anal_fcn_tree_insert (&anal->fcn_tree, fcn);
				r_list_append (anal->fcns, fcn);
			}
		} // End of methods loop
	}// end of bin_objs list loop
	return result;
}