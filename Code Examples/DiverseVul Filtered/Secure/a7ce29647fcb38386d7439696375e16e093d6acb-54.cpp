R_API int r_anal_var_count(RAnal *a, RAnalFunction *fcn, int kind, int type) {
	r_return_val_if_fail (fcn && a && type >= 0 && type <= 1, -1);
	// type { local: 0, arg: 1 };
	RList *list = r_anal_var_list (a, fcn, kind);
	RAnalVar *var;
	RListIter *iter;
	int count[2] = {
		0
	};
	r_list_foreach (list, iter, var) {
		if (kind == R_ANAL_VAR_KIND_REG) {
			count[1]++;
			continue;
		}
		count[var->isarg]++;
	}
	r_list_free (list);
	return count[type];
}