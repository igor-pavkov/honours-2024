static int handle_bb_cf_recursive_descent (RAnal *anal, RAnalState *state) {

	RAnalBlock *bb = state->current_bb;

	ut64 addr = 0;
	int result = 0;
	if (!bb) {
		eprintf ("Error: unable to handle basic block @ 0x%08"PFMT64x"\n", addr);
		return R_ANAL_RET_ERROR;
	} else if (state->max_depth <= state->current_depth) {
		return R_ANAL_RET_ERROR;
	}

	state->current_depth++;
	addr = bb->addr;
	IFDBG eprintf ("Handling a control flow change @ 0x%04"PFMT64x".\n", addr);
	ut64 control_type = r_anal_ex_map_anal_ex_to_anal_op_type (bb->type2);

	// XXX - transition to type2 control flow condtions
	switch (control_type) {
		case R_ANAL_OP_TYPE_CALL:
			IFDBG eprintf (" - Handling a call @ 0x%04"PFMT64x".\n", addr);
			r_anal_xrefs_set (anal, bb->addr, bb->jump, R_ANAL_REF_TYPE_CALL);
			result = R_ANAL_RET_ERROR;
			break;
		case R_ANAL_OP_TYPE_JMP:
			{
				RList * jmp_list;
				IFDBG eprintf (" - Handling a jmp @ 0x%04"PFMT64x" to 0x%04"PFMT64x".\n", addr, bb->jump);

				// visited some other time
				if (!r_anal_state_search_bb (state, bb->jump)) {
					jmp_list = r_anal_ex_perform_analysis ( anal, state, bb->jump );
					if (jmp_list)
						bb->jumpbb = (RAnalBlock *) r_list_get_n (jmp_list, 0);
					if (bb->jumpbb)
						bb->jump = bb->jumpbb->addr;
				} else {
					bb->jumpbb = r_anal_state_search_bb (state, bb->jump);
					if (bb->jumpbb)
						bb->jump = bb->jumpbb->addr;
				}

				if (state->done == 1) {
					IFDBG eprintf (" Looks like this jmp (bb @ 0x%04"PFMT64x") found a return.\n", addr);
				}
				result = R_ANAL_RET_END;
			}
			break;
		case R_ANAL_OP_TYPE_CJMP:
			{
				RList *jmp_list;
				ut8 encountered_stop = 0;
				IFDBG eprintf (" - Handling a cjmp @ 0x%04"PFMT64x" jmp to 0x%04"PFMT64x" and fail to 0x%04"PFMT64x".\n", addr, bb->jump, bb->fail);
				IFDBG eprintf (" - Handling jmp to 0x%04"PFMT64x".\n", bb->jump);
				// visited some other time
				if (!r_anal_state_search_bb (state, bb->jump)) {
					jmp_list = r_anal_ex_perform_analysis ( anal, state, bb->jump );
					if (jmp_list)
						bb->jumpbb = (RAnalBlock *) r_list_get_n (jmp_list, 0);
					if (bb->jumpbb) {
						bb->jump = bb->jumpbb->addr;
					}
				} else {
					bb->jumpbb = r_anal_state_search_bb (state, bb->jump);
					bb->jump = bb->jumpbb->addr;
				}

				if (state->done == 1) {
					IFDBG eprintf (" Looks like this jmp (bb @ 0x%04"PFMT64x") found a return.\n", addr);
					state->done = 0;
					encountered_stop = 1;
				}

				if (!r_anal_state_search_bb (state, bb->fail)) {
					jmp_list = r_anal_ex_perform_analysis ( anal, state, bb->fail );
					if (jmp_list)
						bb->failbb = (RAnalBlock *) r_list_get_n (jmp_list, 0);
					if (bb->failbb) {
						bb->fail = bb->failbb->addr;
					}
				} else {
					bb->failbb = r_anal_state_search_bb (state, bb->fail);
					if (bb->failbb) {
						bb->fail = bb->failbb->addr;
					}
				}

				IFDBG eprintf (" - Handling an cjmp @ 0x%04"PFMT64x" jmp to 0x%04"PFMT64x" and fail to 0x%04"PFMT64x".\n", addr, bb->jump, bb->fail);
				IFDBG eprintf (" - Handling fail to 0x%04"PFMT64x".\n", bb->fail);
				// r_anal_state_merge_bb_list (state, fail_list);
				if (state->done == 1) {
					IFDBG eprintf (" Looks like this fail (bb @ 0x%04"PFMT64x") found a return.\n", addr);
				}

				result = R_ANAL_RET_END;
				if (encountered_stop) state->done = 1;
			}
			break;

		case R_ANAL_OP_TYPE_SWITCH:
		{
			IFDBG eprintf (" - Handling an switch @ 0x%04"PFMT64x".\n", addr);
			if (bb->switch_op) {
				RAnalCaseOp *caseop;
				RListIter *iter;
				RList *jmp_list = NULL;
				ut8 encountered_stop = 0;
				r_list_foreach (bb->switch_op->cases, iter, caseop) {
					if (caseop) {
						if (r_anal_state_addr_is_valid (state, caseop->jump) ) {
							jmp_list = r_anal_ex_perform_analysis ( anal, state, caseop->jump );
							if (jmp_list)
								caseop->jumpbb = (RAnalBlock *) r_list_get_n (jmp_list, 0);
							if (state->done == 1) {
								IFDBG eprintf (" Looks like this jmp (bb @ 0x%04"PFMT64x") found a return.\n", addr);
								state->done = 0;
								encountered_stop = 1;
							}
						}
					}
				}
				r_list_free (jmp_list);
				if (encountered_stop) state->done = 1;
			}

			result = R_ANAL_RET_END;
		}
			break;
		case R_ANAL_OP_TYPE_TRAP:
		case R_ANAL_OP_TYPE_UJMP:
		case R_ANAL_OP_TYPE_IJMP:
		case R_ANAL_OP_TYPE_RJMP:
		case R_ANAL_OP_TYPE_IRJMP:
		case R_ANAL_OP_TYPE_RET:
		case R_ANAL_OP_TYPE_ILL:
			IFDBG eprintf (" - Handling an ret @ 0x%04"PFMT64x".\n", addr);
			state->done = 1;
			result = R_ANAL_RET_END;
			break;
		default: break;
	}

	state->current_depth--;
	return result;
}