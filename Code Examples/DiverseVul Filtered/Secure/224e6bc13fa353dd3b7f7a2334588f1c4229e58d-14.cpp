static int handle_bb_cf_linear_sweep (RAnal *anal, RAnalState *state) {
	ut64 * paddr64;
	RAnalBlock *bb = state->current_bb;
	RAnalJavaLinearSweep *nodes = state->user_state;

	if (!nodes || !nodes->cfg_node_addrs) {
		state->done = 1;
		return R_ANAL_RET_ERROR;
	}

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
	ut32 control_type = r_anal_ex_map_anal_ex_to_anal_op_type (bb->type2);

	// XXX - transition to type2 control flow condtions
	switch (control_type) {
		case R_ANAL_OP_TYPE_CALL:
			IFDBG eprintf (" - Handling a call @ 0x%04"PFMT64x"\n", addr);
			r_anal_xrefs_set (anal, bb->addr, bb->jump, R_ANAL_REF_TYPE_CALL);
			result = R_ANAL_RET_ERROR;
			break;
		case R_ANAL_OP_TYPE_JMP:
			paddr64 = malloc (sizeof(ut64));
			*paddr64 = bb->jump;
			IFDBG eprintf (" - Handling a jmp @ 0x%04"PFMT64x", adding for future visit\n", addr);
			r_list_append (nodes->cfg_node_addrs, paddr64);
			result = R_ANAL_RET_END;
			break;
		case R_ANAL_OP_TYPE_CJMP:
			paddr64 = malloc (sizeof(ut64));
			*paddr64 = bb->jump;
			IFDBG eprintf (" - Handling a bb->jump @ 0x%04"PFMT64x", adding 0x%04"PFMT64x" for future visit\n", addr, *paddr64);
			r_list_append (nodes->cfg_node_addrs, paddr64);
			paddr64 = malloc (sizeof(ut64));
			if (paddr64) {
				*paddr64 = bb->fail;
				IFDBG eprintf (" - Handling a bb->fail @ 0x%04"PFMT64x", adding 0x%04"PFMT64x" for future visit\n", addr, *paddr64);
				r_list_append (nodes->cfg_node_addrs, paddr64);
			}
			result = R_ANAL_RET_END;
			break;
		case R_ANAL_OP_TYPE_SWITCH:
			if (bb->switch_op) {
				RAnalCaseOp *caseop;
				RListIter *iter;
				//RList *jmp_list = NULL;
				IFDBG eprintf (" - Handling a switch_op @ 0x%04"PFMT64x":\n", addr);
				r_list_foreach (bb->switch_op->cases, iter, caseop) {
					ut64 * paddr64;
					if (caseop) {
						paddr64 = malloc (sizeof(ut64));
						*paddr64 = caseop->jump;
						IFDBG eprintf ("Adding 0x%04"PFMT64x" for future visit\n", *paddr64);
						r_list_append (nodes->cfg_node_addrs, paddr64);
					}
				}
			}
			result = R_ANAL_RET_END;
			break;
		case R_ANAL_OP_TYPE_TRAP:
		case R_ANAL_OP_TYPE_UJMP:
		case R_ANAL_OP_TYPE_RJMP:
		case R_ANAL_OP_TYPE_IJMP:
		case R_ANAL_OP_TYPE_IRJMP:
		case R_ANAL_OP_TYPE_RET:
			IFDBG eprintf (" - Handling an ret @ 0x%04"PFMT64x".\n", addr);
			state->done = 1;
			result = R_ANAL_RET_END;
			break;
		default: break;
	}

	state->current_depth--;
	return result;
}