static void __generic_sub_update_flags_rk(RAnalOp *op, int d, int k, int carry) {
	__generic_sub_update_flags(op, 'r', d, 'k', k, carry);
}