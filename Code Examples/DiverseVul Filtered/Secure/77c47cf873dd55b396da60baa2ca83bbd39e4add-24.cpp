static int first_nibble_is_0(RAnal* anal, RAnalOp* op, ut16 code){
	if(IS_BSRF(code)) {
		/* Call 'far' subroutine Rn+PC+4 */
		op->type = R_ANAL_OP_TYPE_UCALL;
		op->delay = 1;
		op->dst = anal_regrel_jump (anal, op, GET_TARGET_REG(code));
	} else if (IS_BRAF(code)) {
		/* Unconditional branch to Rn+PC+4, no delay slot */
		op->type = R_ANAL_OP_TYPE_UJMP;
		op->dst = anal_regrel_jump (anal, op, GET_TARGET_REG(code));
		op->eob = true;
	} else if( IS_RTS(code) ) {
		/* Ret from subroutine. Returns to pr */
		//TODO Convert into jump pr?
		op->type = R_ANAL_OP_TYPE_RET;
		op->delay = 1;
		op->eob = true;
	} else if (IS_RTE(code)) {
		//TODO Convert into jmp spc? Indicate ssr->sr as well?
		op->type = R_ANAL_OP_TYPE_RET;
		op->delay = 1;
		op->eob = true;
	} else if (IS_MOVB_REG_TO_R0REL(code)) {	//0000nnnnmmmm0100 mov.b <REG_M>,@(R0,<REG_N>)
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_r0_reg_ref (anal, GET_TARGET_REG(code), BYTE_SIZE);
	} else if (IS_MOVW_REG_TO_R0REL(code)) {
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_r0_reg_ref (anal, GET_TARGET_REG(code), WORD_SIZE);
	} else if (IS_MOVL_REG_TO_R0REL(code)) {
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_r0_reg_ref(anal, GET_TARGET_REG(code), LONG_SIZE);
	} else if (IS_MOVB_R0REL_TO_REG(code)) {
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->src[0] = anal_fill_r0_reg_ref (anal, GET_SOURCE_REG(code), BYTE_SIZE);
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_MOVW_R0REL_TO_REG(code)) {
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->src[0] = anal_fill_r0_reg_ref (anal, GET_SOURCE_REG(code), WORD_SIZE);
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_MOVL_R0REL_TO_REG(code)) {
		op->type = R_ANAL_OP_TYPE_LOAD;
		op->src[0] = anal_fill_r0_reg_ref (anal, GET_SOURCE_REG(code), LONG_SIZE);
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_NOP(code)) {
		op->type = R_ANAL_OP_TYPE_NOP;
	} else if (IS_CLRT(code)) {
		op->type = R_ANAL_OP_TYPE_UNK;	//TODO : implement flag
	} else if (IS_SETT(code)) {
		op->type = R_ANAL_OP_TYPE_UNK;
	} else if (IS_CLRMAC(code)) {
		op->type = R_ANAL_OP_TYPE_UNK;	//TODO : type_mov ?
	} else if (IS_DIV0U(code)) {
		op->type = R_ANAL_OP_TYPE_DIV;
	} else if (IS_MOVT(code)) {
		op->type = R_ANAL_OP_TYPE_MOV;
		//op->src[0] = 		//TODO: figure out how to get T flag from sr reg
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_MULL(code)) {
		op->type = R_ANAL_OP_TYPE_MUL;
		op->src[0] = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		op->src[1] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		//op->dst = //TODO: figure out how to set MACL + MACH
	} else if (IS_SLEEP(code)) {
		op->type = R_ANAL_OP_TYPE_UNK;
	} else if (IS_STSMAC(code)) {	//0000nnnn0000101_ sts MAC*,<REG_N>
		op->type = R_ANAL_OP_TYPE_MOV;
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_STCSR1(code)) {	//0000nnnn00010010 stc {sr,gbr,vbr,ssr},<REG_N>
		op->type = R_ANAL_OP_TYPE_MOV;
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		//todo: plug in src
	} else if (IS_STSPR(code)) {	//0000nnnn00101010 sts PR,<REG_N>
		op->type = R_ANAL_OP_TYPE_MOV;
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		//todo: plug in src
	}

	//TODO Check missing insns, especially STC might be interesting
	return op->size;
}