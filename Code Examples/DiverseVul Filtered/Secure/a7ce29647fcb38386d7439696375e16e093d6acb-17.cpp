R_API void r_anal_var_remove_access_at(RAnalVar *var, ut64 address) {
	r_return_if_fail (var);
	st64 offset = address - var->fcn->addr;
	size_t index;
	r_vector_lower_bound (&var->accesses, offset, index, ACCESS_CMP);
	if (index >= var->accesses.len) {
		return;
	}
	RAnalVarAccess *acc = r_vector_index_ptr (&var->accesses, index);
	if (acc->offset == offset) {
		r_vector_remove_at (&var->accesses, index, NULL);
		RPVector *inst_accesses = ht_up_find (var->fcn->inst_vars, (ut64)offset, NULL);
		r_pvector_remove_data (inst_accesses, var);
	}
	R_DIRTY (var->fcn->anal);
}