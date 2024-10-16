static int fpu_insn(RAnal* anal, RAnalOp* op, ut16 code){
	//Not interested on FPU stuff for now
	op->family = R_ANAL_OP_FAMILY_FPU;
	return op->size;
}