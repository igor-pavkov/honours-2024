void xfrm_policy_unregister_afinfo(const struct xfrm_policy_afinfo *afinfo)
{
	struct dst_ops *dst_ops = afinfo->dst_ops;
	int i;

	for (i = 0; i < ARRAY_SIZE(xfrm_policy_afinfo); i++) {
		if (xfrm_policy_afinfo[i] != afinfo)
			continue;
		RCU_INIT_POINTER(xfrm_policy_afinfo[i], NULL);
		break;
	}

	synchronize_rcu();

	dst_ops->kmem_cachep = NULL;
	dst_ops->check = NULL;
	dst_ops->negative_advice = NULL;
	dst_ops->link_failure = NULL;
}