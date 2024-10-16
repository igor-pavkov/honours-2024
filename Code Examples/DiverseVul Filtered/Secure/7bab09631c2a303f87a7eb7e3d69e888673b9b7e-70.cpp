static void xfrm_idx_hash_transfer(struct hlist_head *list,
				   struct hlist_head *nidxtable,
				   unsigned int nhashmask)
{
	struct hlist_node *tmp;
	struct xfrm_policy *pol;

	hlist_for_each_entry_safe(pol, tmp, list, byidx) {
		unsigned int h;

		h = __idx_hash(pol->index, nhashmask);
		hlist_add_head(&pol->byidx, nidxtable+h);
	}
}