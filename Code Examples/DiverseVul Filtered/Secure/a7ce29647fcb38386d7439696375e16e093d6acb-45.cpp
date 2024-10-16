R_API RAnalVarAccess *r_anal_var_get_access_at(RAnalVar *var, ut64 addr) {
	r_return_val_if_fail (var, NULL);
	st64 offset = addr - var->fcn->addr;
	size_t index;
	r_vector_lower_bound (&var->accesses, offset, index, ACCESS_CMP);
	if (index >= var->accesses.len) {
		return NULL;
	}
	RAnalVarAccess *acc = r_vector_index_ptr (&var->accesses, index);
	if (acc->offset == offset) {
		return acc;
	}
	return NULL;
}