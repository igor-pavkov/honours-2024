static bool xfrm_policy_mark_match(struct xfrm_policy *policy,
				   struct xfrm_policy *pol)
{
	u32 mark = policy->mark.v & policy->mark.m;

	if (policy->mark.v == pol->mark.v && policy->mark.m == pol->mark.m)
		return true;

	if ((mark & pol->mark.m) == pol->mark.v &&
	    policy->priority == pol->priority)
		return true;

	return false;
}