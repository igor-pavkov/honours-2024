static int movl_pcdisp_reg(RAnal* anal, RAnalOp* op, ut16 code){
	op->type = R_ANAL_OP_TYPE_LOAD;
	op->src[0] = anal_pcrel_disp_mov (anal, op, code&0xFF, LONG_SIZE);
	op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	return op->size;
}