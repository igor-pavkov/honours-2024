R_API void r_anal_extract_vars(RAnal *anal, RAnalFunction *fcn, RAnalOp *op) {
	r_return_if_fail (anal && fcn && op);

	const char *BP = anal->reg->name[R_REG_NAME_BP];
	if (BP) {
		extract_arg (anal, fcn, op, BP, "+", R_ANAL_VAR_KIND_BPV);
		extract_arg (anal, fcn, op, BP, "-", R_ANAL_VAR_KIND_BPV);
	}
	const char *SP = anal->reg->name[R_REG_NAME_SP];
	if (SP) {
		extract_arg (anal, fcn, op, SP, "+", R_ANAL_VAR_KIND_SPV);
	}
}