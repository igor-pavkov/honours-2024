R_API ut16 r_anal_bb_offset_inst(RAnalBlock *bb, int i) {
	if (i < 0 || i >= bb->ninstr) {
		return UT16_MAX;
	}
	return (i > 0 && (i - 1) < bb->op_pos_size) ? bb->op_pos[i - 1] : 0;
}