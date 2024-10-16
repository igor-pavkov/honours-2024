static RList *var_generate_list(RAnal *a, RAnalFunction *fcn, int kind) {
	if (!a || !fcn) {
		return NULL;
	}
	RList *list = r_list_new ();
	if (kind < 1) {
		kind = R_ANAL_VAR_KIND_BPV; // by default show vars
	}
	if (fcn->vars.v.len > 0) {
		void **it;
		r_pvector_foreach (&fcn->vars, it) {
			RAnalVar *var = *it;
			if (var->kind == kind) {
				r_list_push (list, var);
			}
		}
	}
	return list;
}