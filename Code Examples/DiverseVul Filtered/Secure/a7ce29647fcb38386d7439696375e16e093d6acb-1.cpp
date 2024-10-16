R_API bool r_anal_function_set_var_prot(RAnalFunction *fcn, RList *l) {
	RListIter *iter;
	RAnalVarProt *vp;
	r_list_foreach (l, iter, vp) {
		if (!r_anal_function_set_var (fcn, vp->delta, vp->kind, vp->type, -1, vp->isarg, vp->name)) {
			return false;
		}
	}
	R_DIRTY (fcn->anal);
	return true;
}