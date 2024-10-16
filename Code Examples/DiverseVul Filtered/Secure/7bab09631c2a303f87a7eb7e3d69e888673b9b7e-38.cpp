void xfrm_policy_destroy(struct xfrm_policy *policy)
{
	BUG_ON(!policy->walk.dead);

	if (del_timer(&policy->timer) || del_timer(&policy->polq.hold_timer))
		BUG();

	call_rcu(&policy->rcu, xfrm_policy_destroy_rcu);
}