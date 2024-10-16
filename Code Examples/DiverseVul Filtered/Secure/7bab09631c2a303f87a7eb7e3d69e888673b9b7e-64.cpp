int xfrm_policy_insert(int dir, struct xfrm_policy *policy, int excl)
{
	struct net *net = xp_net(policy);
	struct xfrm_policy *pol;
	struct xfrm_policy *delpol;
	struct hlist_head *chain;
	struct hlist_node *newpos;

	spin_lock_bh(&net->xfrm.xfrm_policy_lock);
	chain = policy_hash_bysel(net, &policy->selector, policy->family, dir);
	delpol = NULL;
	newpos = NULL;
	hlist_for_each_entry(pol, chain, bydst) {
		if (pol->type == policy->type &&
		    !selector_cmp(&pol->selector, &policy->selector) &&
		    xfrm_policy_mark_match(policy, pol) &&
		    xfrm_sec_ctx_match(pol->security, policy->security) &&
		    !WARN_ON(delpol)) {
			if (excl) {
				spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
				return -EEXIST;
			}
			delpol = pol;
			if (policy->priority > pol->priority)
				continue;
		} else if (policy->priority >= pol->priority) {
			newpos = &pol->bydst;
			continue;
		}
		if (delpol)
			break;
	}
	if (newpos)
		hlist_add_behind(&policy->bydst, newpos);
	else
		hlist_add_head(&policy->bydst, chain);
	__xfrm_policy_link(policy, dir);
	atomic_inc(&net->xfrm.flow_cache_genid);

	/* After previous checking, family can either be AF_INET or AF_INET6 */
	if (policy->family == AF_INET)
		rt_genid_bump_ipv4(net);
	else
		rt_genid_bump_ipv6(net);

	if (delpol) {
		xfrm_policy_requeue(delpol, policy);
		__xfrm_policy_unlink(delpol, dir);
	}
	policy->index = delpol ? delpol->index : xfrm_gen_index(net, dir, policy->index);
	hlist_add_head(&policy->byidx, net->xfrm.policy_byidx+idx_hash(net, policy->index));
	policy->curlft.add_time = get_seconds();
	policy->curlft.use_time = 0;
	if (!mod_timer(&policy->timer, jiffies + HZ))
		xfrm_pol_hold(policy);
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);

	if (delpol)
		xfrm_policy_kill(delpol);
	else if (xfrm_bydst_should_resize(net, dir, NULL))
		schedule_work(&net->xfrm.policy_hash_work);

	return 0;
}