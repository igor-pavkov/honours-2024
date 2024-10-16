static RAnalVar *get_stack_var(RAnalFunction *fcn, int delta) {
	void **it;
	r_pvector_foreach (&fcn->vars, it) {
		RAnalVar *var = *it;
		bool is_stack = var->kind == R_ANAL_VAR_KIND_SPV || var->kind == R_ANAL_VAR_KIND_BPV;
		if (is_stack && var->delta == delta) {
			return var;
		}
	}
	return NULL;
}