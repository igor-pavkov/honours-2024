R_API RAnalBlock *r_anal_bb_new() {
	RAnalBlock *bb = R_NEW0 (RAnalBlock);
	if (!bb) {
		return NULL;
	}
	bb->addr = UT64_MAX;
	bb->jump = UT64_MAX;
	bb->fail = UT64_MAX;
	bb->switch_op = NULL;
	bb->type = R_ANAL_BB_TYPE_NULL;
	bb->cond = NULL;
	bb->fingerprint = NULL;
	bb->diff = NULL; //r_anal_diff_new ();
	bb->label = NULL;
	bb->op_pos = R_NEWS0 (ut16, DFLT_NINSTR);
	bb->op_pos_size = DFLT_NINSTR;
	bb->parent_reg_arena = NULL;
	bb->stackptr = 0;
	bb->parent_stackptr = INT_MAX;
	return bb;
}