static inline bool xfrm_pol_hold_rcu(struct xfrm_policy *policy)
{
	return refcount_inc_not_zero(&policy->refcnt);
}