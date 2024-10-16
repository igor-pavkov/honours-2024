static int movw_pcdisp_reg(RAnal* anal, RAnalOp* op, ut16 code){
	op->type = R_ANAL_OP_TYPE_LOAD;
	op->dst = anal_fill_ai_rg (anal, GET_TARGET_REG(code));
	//op->src[0] = anal_fill_reg_disp_mem(anal,PC_IDX,code&0xFF,WORD_SIZE);	//XXX trash in 2 commits
	op->src[0] = anal_pcrel_disp_mov (anal, op, code&0xFF, WORD_SIZE);
	return op->size;
}