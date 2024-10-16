R_API RAnalVar *r_anal_var_get_dst_var(RAnalVar *var) {
	r_return_val_if_fail (var, NULL);
	RAnalVarAccess *acc;
	r_vector_foreach (&var->accesses, acc) {
		if (!(acc->type & R_ANAL_VAR_ACCESS_TYPE_READ)) {
			continue;
		}
		ut64 addr = var->fcn->addr + acc->offset;
		RPVector *used_vars = r_anal_function_get_vars_used_at (var->fcn, addr);
		void **it;
		r_pvector_foreach (used_vars, it) {
			RAnalVar *used_var = *it;
			if (used_var == var) {
				continue;
			}
			RAnalVarAccess *other_acc = r_anal_var_get_access_at (used_var, addr);
			if (other_acc && other_acc->type & R_ANAL_VAR_ACCESS_TYPE_WRITE) {
				return used_var;
			}
		}
	}
	return NULL;
}