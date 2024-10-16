R_API void r_anal_function_delete_vars_by_kind(RAnalFunction *fcn, RAnalVarKind kind) {
	r_return_if_fail (fcn);
	size_t i;
	for (i = 0; i < r_pvector_len (&fcn->vars);) {
		RAnalVar *var = r_pvector_at (&fcn->vars, i);
		if (var->kind == kind) {
			r_pvector_remove_at (&fcn->vars, i);
			var_free (var);
			continue;
		}
		i++;
	}
}