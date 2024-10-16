R_API void r_anal_function_delete_var(RAnalFunction *fcn, RAnalVar *var) {
	r_return_if_fail (fcn && var);
	r_pvector_remove_data (&fcn->vars, var);
	var_free (var);
}