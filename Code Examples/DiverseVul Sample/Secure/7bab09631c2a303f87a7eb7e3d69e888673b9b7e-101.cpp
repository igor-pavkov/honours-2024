struct xfrm_policy *xfrm_policy_alloc(struct net *net, gfp_t gfp)
{
	struct xfrm_policy *policy;

	policy = kzalloc(sizeof(struct xfrm_policy), gfp);

	if (policy) {
		write_pnet(&policy->xp_net, net);
		INIT_LIST_HEAD(&policy->walk.all);
		INIT_HLIST_NODE(&policy->bydst);
		INIT_HLIST_NODE(&policy->byidx);
		rwlock_init(&policy->lock);
		refcount_set(&policy->refcnt, 1);
		skb_queue_head_init(&policy->polq.hold_queue);
		setup_timer(&policy->timer, xfrm_policy_timer,
				(unsigned long)policy);
		setup_timer(&policy->polq.hold_timer, xfrm_policy_queue_process,
			    (unsigned long)policy);
		policy->flo.ops = &xfrm_policy_fc_ops;
	}
	return policy;
}