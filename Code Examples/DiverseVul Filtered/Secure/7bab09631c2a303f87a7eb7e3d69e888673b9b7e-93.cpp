static void __xfrm_policy_link(struct xfrm_policy *pol, int dir)
{
	struct net *net = xp_net(pol);

	list_add(&pol->walk.all, &net->xfrm.policy_all);
	net->xfrm.policy_count[dir]++;
	xfrm_pol_hold(pol);
}