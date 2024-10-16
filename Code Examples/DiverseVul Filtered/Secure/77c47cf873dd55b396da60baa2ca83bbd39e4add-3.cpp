static int first_nibble_is_2(RAnal* anal, RAnalOp* op, ut16 code){
	if (IS_MOVB_REG_TO_REGREF(code)) {	// 0010nnnnmmmm0000 mov.b <REG_M>,@<REG_N>
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_reg_ref (anal, GET_TARGET_REG(code), BYTE_SIZE);
	} else if (IS_MOVW_REG_TO_REGREF(code)) {
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_reg_ref (anal, GET_TARGET_REG(code), WORD_SIZE);
	} else if (IS_MOVL_REG_TO_REGREF(code)) {
		op->type = R_ANAL_OP_TYPE_STORE;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_reg_ref (anal, GET_TARGET_REG(code), LONG_SIZE);
	} else if (IS_AND_REGS(code)) {
		op->type = R_ANAL_OP_TYPE_AND;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_XOR_REGS(code)) {
		op->type = R_ANAL_OP_TYPE_XOR;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_OR_REGS(code)) {
		op->type = R_ANAL_OP_TYPE_OR;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_PUSHB(code) || IS_PUSHW(code) || IS_PUSHL(code)) {
		op->type = R_ANAL_OP_TYPE_PUSH;
		//TODO Handle 'pushes' (mov Rm,@-Rn)
	} else if (IS_TSTRR(code)) {
		op->type = R_ANAL_OP_TYPE_ACMP;
		//TODO: handle tst reg,reg
	} else if (IS_CMPSTR(code)) {	//0010nnnnmmmm1100 cmp/str <REG_M>,<REG_N>
		op->type = R_ANAL_OP_TYPE_ACMP;	//maybe not?
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->src[1] = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		//todo: handle cmp/str byte-per-byte cmp?
	} else if (IS_XTRCT(code)) {	//0010nnnnmmmm1101 xtrct <REG_M>,<REG_N>
		op->type = R_ANAL_OP_TYPE_MOV;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->src[1] = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		//todo: add details ?
	} else if (IS_DIV0S(code)) {
		op->type = R_ANAL_OP_TYPE_DIV;
		//todo: add details?
	} else if (IS_MULUW(code) || IS_MULSW(code)) {	//0010nnnnmmmm111_ mul{s,u}.w <REG_M>,<REG_N>
		op->type = R_ANAL_OP_TYPE_MUL;
		op->src[0] = anal_fill_ai_rg(anal,GET_SOURCE_REG(code));
		op->src[1] = anal_fill_ai_rg(anal,GET_TARGET_REG(code));
		//todo: dest=MACL
	}

	return op->size;
}