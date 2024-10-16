static int add_imm(RAnal* anal, RAnalOp* op, ut16 code){
	op->type = R_ANAL_OP_TYPE_ADD;
	op->src[0] = anal_fill_im (anal, (st8)(code&0xFF)); //Casting to (st8) forces sign-extension.
	op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	return op->size;
}