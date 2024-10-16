static u32 xfrm_gen_index(struct net *net, int dir, u32 index)
{
	static u32 idx_generator;

	for (;;) {
		struct hlist_head *list;
		struct xfrm_policy *p;
		u32 idx;
		int found;

		if (!index) {
			idx = (idx_generator | dir);
			idx_generator += 8;
		} else {
			idx = index;
			index = 0;
		}

		if (idx == 0)
			idx = 8;
		list = net->xfrm.policy_byidx + idx_hash(net, idx);
		found = 0;
		hlist_for_each_entry(p, list, byidx) {
			if (p->index == idx) {
				found = 1;
				break;
			}
		}
		if (!found)
			return idx;
	}
}