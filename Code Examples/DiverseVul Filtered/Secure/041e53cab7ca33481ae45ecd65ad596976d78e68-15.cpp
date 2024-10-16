static void __generic_sub_update_flags_rr(RAnalOp *op, int d, int r, int carry) {
	__generic_sub_update_flags(op, 'r', d, 'r', r, carry);
}