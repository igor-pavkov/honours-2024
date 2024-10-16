static int java_post_anal_linear_sweep(RAnal *anal, RAnalState *state, ut64 addr) {
	RAnalJavaLinearSweep *nodes = state->user_state;
	RList *jmp_list = NULL;
	ut64 *paddr64;

	state->done = 0;
	if (!nodes || !nodes->cfg_node_addrs) {
		state->done = 1;
		return R_ANAL_RET_ERROR;
	}

	while (r_list_length (nodes->cfg_node_addrs) > 0) {
		paddr64 = r_list_get_n (nodes->cfg_node_addrs, 0);
		r_list_del_n (nodes->cfg_node_addrs, 0);
		if (paddr64 && !r_anal_state_search_bb (state, *paddr64)) {
			ut64 list_length = 0;
			IFDBG eprintf (" - Visiting 0x%04"PFMT64x" for analysis.\n", *paddr64);
			jmp_list = r_anal_ex_perform_analysis ( anal, state, *paddr64 );
			list_length = r_list_length (jmp_list);
			r_list_free (jmp_list);
			if ( list_length > 0) {
				IFDBG eprintf (" - Found %"PFMT64d" more basic blocks missed on the initial pass.\n", *paddr64);
			}
		}

	}
	return R_ANAL_RET_END;
}