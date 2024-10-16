R_API void r_anal_var_clear_accesses(RAnalVar *var) {
	r_return_if_fail (var);
	RAnalFunction *fcn = var->fcn;
	if (fcn->inst_vars) {
		// remove all inverse references to the var's accesses
		RAnalVarAccess *acc;
		r_vector_foreach (&var->accesses, acc) {
			RPVector *inst_accesses = ht_up_find (fcn->inst_vars, (ut64)acc->offset, NULL);
			if (!inst_accesses) {
				continue;
			}
			r_pvector_remove_data (inst_accesses, var);
		}
	}
	r_vector_clear (&var->accesses);
	R_DIRTY (var->fcn->anal);
}