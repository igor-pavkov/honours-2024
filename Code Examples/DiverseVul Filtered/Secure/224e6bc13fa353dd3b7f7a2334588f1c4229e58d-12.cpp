static int java_revisit_bb_anal_recursive_descent(RAnal *anal, RAnalState *state, ut64 addr) {
	RAnalBlock *current_head = state && state->current_bb_head ? state->current_bb_head : NULL;
	if (current_head && state->current_bb &&
		state->current_bb->type & R_ANAL_BB_TYPE_TAIL) {
		r_anal_ex_update_bb_cfg_head_tail (current_head, current_head, state->current_bb);
		// XXX should i do this instead -> r_anal_ex_perform_post_anal_bb_cb (anal, state, addr+offset);
		state->done = 1;
	}
	return R_ANAL_RET_END;
}