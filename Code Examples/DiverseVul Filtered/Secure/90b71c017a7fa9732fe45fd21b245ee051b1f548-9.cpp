R_API bool r_anal_bb_set_offset(RAnalBlock *bb, int i, ut16 v) {
	// the offset 0 of the instruction 0 is not stored because always 0
	if (i > 0 && v > 0) {
		if (i >= bb->op_pos_size) {
			int new_pos_size = i * 2;
			ut16 *tmp_op_pos = realloc (bb->op_pos, new_pos_size * sizeof (*bb->op_pos));
			if (!tmp_op_pos) {
				return false;
			}
			bb->op_pos_size = new_pos_size;
			bb->op_pos = tmp_op_pos;
		}
		bb->op_pos[i - 1] = v;
		return true;
	}
	return true;
}