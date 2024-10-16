R_API RAnalBlock *r_anal_bb_get_jumpbb(RAnalFunction *fcn, RAnalBlock *bb) {
	if (bb->jump == UT64_MAX) {
		return NULL;
	}
	if (bb->jumpbb) {
		return bb->jumpbb;
	}
	RListIter *iter;
	RAnalBlock *b;
	r_list_foreach (fcn->bbs, iter, b) {
		if (b->addr == bb->jump) {
			bb->jumpbb = b;
			b->prev = bb;
			return b;
		}
	}
	return NULL;
}