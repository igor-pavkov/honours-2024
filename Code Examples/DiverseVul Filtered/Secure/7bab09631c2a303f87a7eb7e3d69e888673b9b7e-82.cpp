static void xfrm_sk_policy_link(struct xfrm_policy *pol, int dir)
{
	__xfrm_policy_link(pol, XFRM_POLICY_MAX + dir);
}