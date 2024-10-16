R_API R_DEPRECATE RAnalVar *r_anal_get_used_function_var(RAnal *anal, ut64 addr) {
	RList *fcns = r_anal_get_functions_in (anal, addr);
	if (!fcns) {
		return NULL;
	}
	RAnalVar *var = NULL;
	RListIter *it;
	RAnalFunction *fcn;
	r_list_foreach (fcns, it, fcn) {
		RPVector *used_vars = r_anal_function_get_vars_used_at (fcn, addr);
		if (used_vars && !r_pvector_empty (used_vars)) {
			var = r_pvector_at (used_vars, 0);
			break;
		}
	}
	r_list_free (fcns);
	return var;
}