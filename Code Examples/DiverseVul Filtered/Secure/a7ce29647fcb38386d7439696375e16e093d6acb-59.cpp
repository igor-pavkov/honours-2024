static bool is_reg_in_src(const char *regname, RAnal *anal, RAnalOp *op) {
	const char* opsreg0 = op->src[0] ? get_regname (anal, op->src[0]) : NULL;
	const char* opsreg1 = op->src[1] ? get_regname (anal, op->src[1]) : NULL;
	const char* opsreg2 = op->src[2] ? get_regname (anal, op->src[2]) : NULL;
	return (STR_EQUAL (regname, opsreg0)) || (STR_EQUAL (regname, opsreg1)) || (STR_EQUAL (regname, opsreg2));
}