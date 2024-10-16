static void xfrm_policy_fini(struct net *net)
{
	unsigned int sz;
	int dir;

	flush_work(&net->xfrm.policy_hash_work);
#ifdef CONFIG_XFRM_SUB_POLICY
	xfrm_policy_flush(net, XFRM_POLICY_TYPE_SUB, false);
#endif
	xfrm_policy_flush(net, XFRM_POLICY_TYPE_MAIN, false);

	WARN_ON(!list_empty(&net->xfrm.policy_all));

	for (dir = 0; dir < XFRM_POLICY_MAX; dir++) {
		struct xfrm_policy_hash *htab;

		WARN_ON(!hlist_empty(&net->xfrm.policy_inexact[dir]));

		htab = &net->xfrm.policy_bydst[dir];
		sz = (htab->hmask + 1) * sizeof(struct hlist_head);
		WARN_ON(!hlist_empty(htab->table));
		xfrm_hash_free(htab->table, sz);
	}

	sz = (net->xfrm.policy_idx_hmask + 1) * sizeof(struct hlist_head);
	WARN_ON(!hlist_empty(net->xfrm.policy_byidx));
	xfrm_hash_free(net->xfrm.policy_byidx, sz);
}