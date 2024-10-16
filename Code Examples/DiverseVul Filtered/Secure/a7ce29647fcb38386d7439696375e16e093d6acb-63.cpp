R_API int r_anal_var_count_all(RAnalFunction *fcn) {
	r_return_val_if_fail (fcn, 0);
	return r_pvector_len (&fcn->vars);
}