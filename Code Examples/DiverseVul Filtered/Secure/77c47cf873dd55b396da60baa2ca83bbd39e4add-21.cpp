static int first_nibble_is_3(RAnal* anal, RAnalOp* op, ut16 code){
	//TODO Handle carry/overflow , CMP/xx?
	if( IS_ADD(code) || IS_ADDC(code) || IS_ADDV(code) ) {
		op->type = R_ANAL_OP_TYPE_ADD;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if ( IS_SUB(code) || IS_SUBC(code) || IS_SUBV(code)) {
		op->type = R_ANAL_OP_TYPE_SUB;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	} else if (IS_CMPEQ(code) || IS_CMPGE(code) || IS_CMPGT(code) ||
				IS_CMPHI(code) || IS_CMPHS(code)) {
		//TODO : finish implementing
		op->type = R_ANAL_OP_TYPE_CMP;
		op->src[0] = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		op->src[1] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
	} else if (IS_DIV1(code)) {
		op->type = R_ANAL_OP_TYPE_DIV;
		op->src[0] = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		op->src[1] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		//todo: dest ?
	} else if (IS_DMULU(code) || IS_DMULS(code)) {
		op->type = R_ANAL_OP_TYPE_MUL;
		op->src[0] = anal_fill_ai_rg (anal, GET_SOURCE_REG(code));
		op->src[1] = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
		//todo: dest=MACL,MACH
	}
	return op->size;
}