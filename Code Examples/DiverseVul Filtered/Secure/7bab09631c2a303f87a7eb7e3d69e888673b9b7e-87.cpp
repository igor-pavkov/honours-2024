static void xfrm_sk_policy_unlink(struct xfrm_policy *pol, int dir)
{
	__xfrm_policy_unlink(pol, XFRM_POLICY_MAX + dir);
}