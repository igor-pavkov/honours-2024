static int mov_imm_reg(RAnal* anal, RAnalOp* op, ut16 code){
	op->type = R_ANAL_OP_TYPE_MOV;
	op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	op->src[0] = anal_fill_im (anal, (st8)(code & 0xFF));
	return op->size;
}