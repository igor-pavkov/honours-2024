static int bsr(RAnal* anal, RAnalOp* op, ut16 code){
	/* Subroutine call, relative to PC */
	op->type = R_ANAL_OP_TYPE_CALL;
	op->jump = disarm_12bit_offset (op, GET_BRA_OFFSET(code));
	op->delay = 1;
	return op->size;
}