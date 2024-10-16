static struct xfrm_policy *xfrm_policy_lookup_bytype(struct net *net, u8 type,
						     const struct flowi *fl,
						     u16 family, u8 dir)
{
	int err;
	struct xfrm_policy *pol, *ret;
	const xfrm_address_t *daddr, *saddr;
	struct hlist_head *chain;
	unsigned int sequence;
	u32 priority;

	daddr = xfrm_flowi_daddr(fl, family);
	saddr = xfrm_flowi_saddr(fl, family);
	if (unlikely(!daddr || !saddr))
		return NULL;

	rcu_read_lock();
 retry:
	do {
		sequence = read_seqcount_begin(&xfrm_policy_hash_generation);
		chain = policy_hash_direct(net, daddr, saddr, family, dir);
	} while (read_seqcount_retry(&xfrm_policy_hash_generation, sequence));

	priority = ~0U;
	ret = NULL;
	hlist_for_each_entry_rcu(pol, chain, bydst) {
		err = xfrm_policy_match(pol, fl, type, family, dir);
		if (err) {
			if (err == -ESRCH)
				continue;
			else {
				ret = ERR_PTR(err);
				goto fail;
			}
		} else {
			ret = pol;
			priority = ret->priority;
			break;
		}
	}
	chain = &net->xfrm.policy_inexact[dir];
	hlist_for_each_entry_rcu(pol, chain, bydst) {
		if ((pol->priority >= priority) && ret)
			break;

		err = xfrm_policy_match(pol, fl, type, family, dir);
		if (err) {
			if (err == -ESRCH)
				continue;
			else {
				ret = ERR_PTR(err);
				goto fail;
			}
		} else {
			ret = pol;
			break;
		}
	}

	if (read_seqcount_retry(&xfrm_policy_hash_generation, sequence))
		goto retry;

	if (ret && !xfrm_pol_hold_rcu(ret))
		goto retry;
fail:
	rcu_read_unlock();

	return ret;
}