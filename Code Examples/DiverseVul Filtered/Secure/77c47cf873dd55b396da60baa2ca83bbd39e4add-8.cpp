static int movl_rdisp_reg(RAnal* anal, RAnalOp* op, ut16 code){
	op->type = R_ANAL_OP_TYPE_LOAD;
	op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	op->src[0] = anal_fill_reg_disp_mem (anal, GET_SOURCE_REG(code), code&0x0F, LONG_SIZE);
	return op->size;
}