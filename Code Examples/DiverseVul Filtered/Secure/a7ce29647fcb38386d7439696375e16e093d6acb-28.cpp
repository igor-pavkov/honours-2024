R_API void r_anal_function_vars_cache_init(RAnal *anal, RAnalFcnVarsCache *cache, RAnalFunction *fcn) {
	cache->bvars = r_anal_var_list (anal, fcn, R_ANAL_VAR_KIND_BPV);
	cache->rvars = r_anal_var_list (anal, fcn, R_ANAL_VAR_KIND_REG);
	cache->svars = r_anal_var_list (anal, fcn, R_ANAL_VAR_KIND_SPV);
	r_list_sort (cache->bvars, (RListComparator)var_comparator);
	RListIter *it;
	RAnalVar *var;
	r_list_foreach (cache->rvars, it, var) {
		var->argnum = r_anal_var_get_argnum (var);
	}
	r_list_sort (cache->rvars, (RListComparator)regvar_comparator);
	r_list_sort (cache->svars, (RListComparator)var_comparator);
}