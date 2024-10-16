R_API void r_anal_var_delete(RAnalVar *var) {
	r_return_if_fail (var);
	RAnalFunction *fcn = var->fcn;
	size_t i;
	for (i = 0; i < r_pvector_len (&fcn->vars); i++) {
		RAnalVar *v = r_pvector_at (&fcn->vars, i);
		if (v == var) {
			r_pvector_remove_at (&fcn->vars, i);
			var_free (v);
			return;
		}
	}
}