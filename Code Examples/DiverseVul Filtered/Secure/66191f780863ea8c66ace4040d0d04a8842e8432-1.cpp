static int opnot(RAsm *a, ut8 * data, const Opcode *op) {
	int l = 0;

	if(op->operands[0].reg == X86R_UNDEFINED)  {
		return -1;
	}

	data[l++] = 0xf7;
	data[l++] = 0xd0 | op->operands[0].reg;

	return l;
}