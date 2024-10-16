static bool is_used_like_arg(const char *regname, const char *opsreg, const char *opdreg, RAnalOp *op, RAnal *anal) {
	RAnalValue *dst = op->dst;
	RAnalValue *src = op->src[0];
	switch (op->type) {
	case R_ANAL_OP_TYPE_POP:
		return false;
	case R_ANAL_OP_TYPE_MOV:
		return (is_reg_in_src (regname, anal, op)) || (STR_EQUAL (opdreg, regname) && dst->memref);
	case R_ANAL_OP_TYPE_CMOV:
		if (STR_EQUAL (opdreg, regname)) {
			return false;
		}
		if (is_reg_in_src (regname, anal, op)) {
			return true;
		}
		return false;
	case R_ANAL_OP_TYPE_LEA:
	case R_ANAL_OP_TYPE_LOAD:
		if (is_reg_in_src (regname, anal, op)) {
			return true;
		}
		if (STR_EQUAL (opdreg, regname)) {
			return false;
		}
    		return false;
	case R_ANAL_OP_TYPE_XOR:
		if (STR_EQUAL (opsreg, opdreg) && !src->memref && !dst->memref) {
			return false;
		}
		//fallthrough
	default:
		if (op_affect_dst (op) && arch_destroys_dst (anal->cur->arch)) {
			if (is_reg_in_src (regname, anal, op)) {
				return true;
			}
			return false;
		}
		return ((STR_EQUAL (opdreg, regname)) || (is_reg_in_src (regname, anal, op)));
	}
}