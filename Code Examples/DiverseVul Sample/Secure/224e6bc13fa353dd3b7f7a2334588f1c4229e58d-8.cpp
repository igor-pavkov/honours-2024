static int java_recursive_descent(RAnal *anal, RAnalState *state, ut64 addr) {
	RAnalBlock *bb;
	RAnalBlock *current_head;

	if (!anal || !state || !state->current_bb || state->current_bb_head)
		return 0;

	bb = state->current_bb;
	current_head = state->current_bb_head;

	if (current_head && state->current_bb->type & R_ANAL_BB_TYPE_TAIL) {
		r_anal_ex_update_bb_cfg_head_tail (current_head, current_head, state->current_bb);
	}

	// basic filter for handling the different type of operations
	// depending on flags some may be called more than once
	// if (bb->type2 & R_ANAL_EX_ILL_OP)   handle_bb_ill_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_COND_OP)  handle_bb_cond_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_UNK_OP)   handle_bb_unknown_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_NULL_OP)  handle_bb_null_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_NOP_OP)   handle_bb_nop_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_REP_OP)   handle_bb_rep_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_STORE_OP) handle_bb_store_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_LOAD_OP)  handle_bb_load_op (anal, state
	// if (bb->type2 & R_ANAL_EX_REG_OP)   handle_bb_reg_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_OBJ_OP)   handle_bb_obj_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_STACK_OP) handle_bb_stack_op (anal, state);
	// if (bb->type2 & R_ANAL_EX_BIN_OP)   handle_bb_bin_op (anal, state);
	if (bb->type2 & R_ANAL_EX_CODE_OP)  handle_bb_cf_recursive_descent (anal, state);
	// if (bb->type2 & R_ANAL_EX_DATA_OP)  handle_bb_data_op (anal, state);
	return 0;
}