static void xfrm_policy_destroy_rcu(struct rcu_head *head)
{
	struct xfrm_policy *policy = container_of(head, struct xfrm_policy, rcu);

	security_xfrm_policy_free(policy->security);
	kfree(policy);
}