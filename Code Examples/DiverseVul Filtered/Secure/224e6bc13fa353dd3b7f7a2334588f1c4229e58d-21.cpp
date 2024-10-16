static int analyze_from_code_buffer(RAnal *anal, RAnalFunction *fcn, ut64 addr, const ut8 *code_buf, ut64 code_length) {
	char gen_name[1025];
	RListIter *bb_iter;
	RAnalBlock *bb;
	ut64 actual_size = 0;
	RAnalState *state = NULL;
	int result = R_ANAL_RET_ERROR;
	RAnalJavaLinearSweep *nodes;

	free (fcn->name);
	free (fcn->dsc);
	snprintf (gen_name, 1024, "sym.%08"PFMT64x"", addr);

	fcn->name = strdup (gen_name);
	fcn->dsc = strdup ("unknown");
	r_anal_fcn_set_size (NULL, fcn, code_length);
	fcn->type = R_ANAL_FCN_TYPE_FCN;
	fcn->addr = addr;
	state = r_anal_state_new (addr, (ut8*) code_buf, code_length);
	nodes = R_NEW0 (RAnalJavaLinearSweep);
	nodes->cfg_node_addrs = r_list_new ();
	nodes->cfg_node_addrs->free = free;
	state->user_state = nodes;
	result = analyze_method (anal, fcn, state);
	r_list_foreach (fcn->bbs, bb_iter, bb) {
		actual_size += bb->size;
	}
	r_anal_fcn_set_size (NULL, fcn, state->bytes_consumed);
	result = state->anal_ret_val;
	r_list_free (nodes->cfg_node_addrs);
	free (nodes);
	//leak to avoid UAF is the easy solution otherwise a whole rewrite is needed
	//r_anal_state_free (state);
	if (r_anal_fcn_size (fcn) != code_length) {
		return R_ANAL_RET_ERROR;
#if 0
		eprintf ("WARNING Analysis of %s Incorrect: Code Length: 0x%"PFMT64x", Function size reported 0x%x\n", fcn->name, code_length, r_anal_fcn_size(fcn));
		eprintf ("Deadcode detected, setting code length to: 0x%"PFMT64x"\n", code_length);
		r_anal_fcn_set_size (fcn, code_length);
#endif
	}
	return result;
}