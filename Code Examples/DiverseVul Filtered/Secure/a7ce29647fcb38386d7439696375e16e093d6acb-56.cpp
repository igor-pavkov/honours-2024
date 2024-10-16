R_API RAnalVar *r_anal_function_get_var(RAnalFunction *fcn, char kind, int delta) {
	r_return_val_if_fail (fcn, NULL);
	void **it;
	r_pvector_foreach (&fcn->vars, it) {
		RAnalVar *var = *it;
		if (var->kind == kind && var->delta == delta) {
			return var;
		}
	}
	return NULL;
}