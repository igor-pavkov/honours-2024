R_API RList *r_anal_var_list(RAnal *a, RAnalFunction *fcn, int kind) {
	return var_generate_list (a, fcn, kind);
}