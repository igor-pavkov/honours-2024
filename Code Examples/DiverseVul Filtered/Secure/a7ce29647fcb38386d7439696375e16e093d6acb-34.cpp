R_API int r_anal_var_count_args(RAnalFunction *fcn) {
	r_return_val_if_fail (fcn, 0); // No function implies no variables, but probably mistake
	int args = 0;
	void **it;
	r_pvector_foreach (&fcn->vars, it) {
		RAnalVar *var = *it;
		if (var->isarg) {
			args++;
		}
	}
	return args;
}