static RAnalValue *anal_pcrel_disp_mov(RAnal* anal, RAnalOp* op, ut8 disp, int size){
	RAnalValue *ret = r_anal_value_new ();
	if (size==2) {
		ret->base = op->addr+4;
		ret->delta = disp<<1;
	} else {
		ret->base = (op->addr+4) & ~0x03;
		ret->delta = disp<<2;
	}

	return ret;
}