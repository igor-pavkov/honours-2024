R_API RAnalBlock *r_anal_bb_get_failbb(RAnalFunction *fcn, RAnalBlock *bb) {
	RListIter *iter;
	RAnalBlock *b;
	if (bb->fail == UT64_MAX) {
		return NULL;
	}
	if (bb->failbb) {
		return bb->failbb;
	}
	r_list_foreach (fcn->bbs, iter, b) {
		if (b->addr == bb->fail) {
			bb->failbb = b;
			b->prev = bb;
			return b;
		}
	}
	return NULL;
}