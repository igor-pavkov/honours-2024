static int first_nibble_is_6(RAnal* anal, RAnalOp* op, ut16 code){
	if (IS_MOV_REGS(code)) {
		op->type = R_ANAL_OP_TYPE_MOV;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_MOVB_REGREF_TO_REG(code)) {
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->src[0] = anal_fill_reg_ref (anal, GET_SOURCE_REG(code), BYTE_SIZE);
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_MOVW_REGREF_TO_REG(code)) {
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->src[0] = anal_fill_reg_ref (anal, GET_SOURCE_REG(code), WORD_SIZE);
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_MOVL_REGREF_TO_REG(code)) {
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->src[0] = anal_fill_reg_ref (anal, GET_SOURCE_REG(code), LONG_SIZE);
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_EXT(code)) {
		//ext{s,u}.{b,w} instructs. todo : more detail ?
		op->type = R_ANAL_OP_TYPE_MOV;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_MOVB_POP(code) || IS_MOVW_POP(code) || IS_MOVL_POP(code)) {
		/* 0110nnnnmmmm0100 mov.b @<REG_M>+,<REG_N>*/
		/* 0110nnnnmmmm0101 mov.w @<REG_M>+,<REG_N>*/
		/* 0110nnnnmmmm0110 mov.l @<REG_M>+,<REG_N>*/
		op->type = R_ANAL_OP_TYPE_POP;
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		//todo : op->src for pop = ?
	} else if (IS_NEG(code)) {
		//todo: neg and negc details
		op->type = R_ANAL_OP_TYPE_UNK;
		/* 0110nnnnmmmm1010 negc*/
		/* 0110nnnnmmmm1010 neg */
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_NOT(code)) {
		//todo : details?
		op->type = R_ANAL_OP_TYPE_NOT;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_SWAP(code)) {
		/* 0110nnnnmmmm1000 swap.b <REG_M>,<REG_N>*/
		/* 0110nnnnmmmm1001 swap.w <REG_M>,<REG_N>*/
		op->type = R_ANAL_OP_TYPE_MOV;
		//todo : details
	}

	return op->size;
}