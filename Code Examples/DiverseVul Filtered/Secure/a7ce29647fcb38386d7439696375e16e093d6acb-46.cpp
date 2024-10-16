R_API void r_anal_var_set_access(RAnalVar *var, const char *reg, ut64 access_addr, int access_type, st64 stackptr) {
	r_return_if_fail (var);
	st64 offset = access_addr - var->fcn->addr;

	// accesses are stored ordered by offset, use binary search to get the matching existing or the index to insert a new one
	size_t index;
	r_vector_lower_bound (&var->accesses, offset, index, ACCESS_CMP);
	RAnalVarAccess *acc = NULL;
	if (index < var->accesses.len) {
		acc = r_vector_index_ptr (&var->accesses, index);
	}
	if (!acc || acc->offset != offset) {
		acc = r_vector_insert (&var->accesses, index, NULL);
		acc->offset = offset;
		acc->type = 0;
	}

	acc->type |= (ut8)access_type;
	acc->stackptr = stackptr;
	acc->reg = r_str_constpool_get (&var->fcn->anal->constpool, reg);

	// add the inverse reference from the instruction to the var
	RPVector *inst_accesses = ht_up_find (var->fcn->inst_vars, (ut64)offset, NULL);
	if (!inst_accesses) {
		inst_accesses = r_pvector_new (NULL);
		if (!inst_accesses) {
			return;
		}
		ht_up_insert (var->fcn->inst_vars, (ut64)offset, inst_accesses);
	}
	if (!r_pvector_contains (inst_accesses, var)) {
		r_pvector_push (inst_accesses, var);
	}
}