static int bra(RAnal* anal, RAnalOp* op, ut16 code){
	/* Unconditional branch, relative to PC */
	op->type = R_ANAL_OP_TYPE_JMP;
	op->delay = 1;
	op->jump = disarm_12bit_offset (op, GET_BRA_OFFSET(code));
	op->eob  = true;
	return op->size;
}