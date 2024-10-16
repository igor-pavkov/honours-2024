R_API R_BORROW RAnalVar *r_anal_function_get_var_byname(RAnalFunction *fcn, const char *name) {
	r_return_val_if_fail (fcn && name, NULL);
	void **it;
	r_pvector_foreach (&fcn->vars, it) {
		RAnalVar *var = *it;
		if (!strcmp (var->name, name)) {
			return var;
		}
	}
	return NULL;
}