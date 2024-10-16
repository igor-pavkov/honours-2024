R_API RList *r_anal_var_all_list(RAnal *anal, RAnalFunction *fcn) {
	// r_anal_var_list if there are not vars with that kind returns a list with
	// zero element.. which is an unnecessary loss of cpu time
	RList *list = r_list_new ();
	if (!list) {
		return NULL;
	}
	RList *reg_vars = r_anal_var_list (anal, fcn, R_ANAL_VAR_KIND_REG);
	RList *bpv_vars = r_anal_var_list (anal, fcn, R_ANAL_VAR_KIND_BPV);
	RList *spv_vars = r_anal_var_list (anal, fcn, R_ANAL_VAR_KIND_SPV);
	r_list_join (list, reg_vars);
	r_list_join (list, bpv_vars);
	r_list_join (list, spv_vars);
	r_list_free (reg_vars);
	r_list_free (bpv_vars);
	r_list_free (spv_vars);
	return list;
}