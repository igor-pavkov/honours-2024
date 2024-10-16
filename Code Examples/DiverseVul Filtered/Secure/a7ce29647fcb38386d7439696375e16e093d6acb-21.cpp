R_API R_BORROW RPVector *r_anal_function_get_vars_used_at(RAnalFunction *fcn, ut64 op_addr) {
	r_return_val_if_fail (fcn, NULL);
	return ht_up_find (fcn->inst_vars, op_addr - fcn->addr, NULL);
}