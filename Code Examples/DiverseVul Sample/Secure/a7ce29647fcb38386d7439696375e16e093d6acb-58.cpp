R_API void r_anal_function_delete_unused_vars(RAnalFunction *fcn) {
	r_return_if_fail (fcn);
	void **v;
	RPVector *vars_clone = (RPVector *)r_vector_clone ((RVector *)&fcn->vars);
	r_pvector_foreach (vars_clone, v) {
		RAnalVar *var = *v;
		if (r_vector_empty (&var->accesses)) {
			r_anal_function_delete_var (fcn, var);
		}
	}
	r_pvector_free (vars_clone);
}