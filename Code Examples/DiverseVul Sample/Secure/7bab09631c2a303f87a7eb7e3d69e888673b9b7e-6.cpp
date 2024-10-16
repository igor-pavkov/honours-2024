int xfrm_policy_flush(struct net *net, u8 type, bool task_valid)
{
	int dir, err = 0, cnt = 0;

	spin_lock_bh(&net->xfrm.xfrm_policy_lock);

	err = xfrm_policy_flush_secctx_check(net, type, task_valid);
	if (err)
		goto out;

	for (dir = 0; dir < XFRM_POLICY_MAX; dir++) {
		struct xfrm_policy *pol;
		int i;

	again1:
		hlist_for_each_entry(pol,
				     &net->xfrm.policy_inexact[dir], bydst) {
			if (pol->type != type)
				continue;
			__xfrm_policy_unlink(pol, dir);
			spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
			cnt++;

			xfrm_audit_policy_delete(pol, 1, task_valid);

			xfrm_policy_kill(pol);

			spin_lock_bh(&net->xfrm.xfrm_policy_lock);
			goto again1;
		}

		for (i = net->xfrm.policy_bydst[dir].hmask; i >= 0; i--) {
	again2:
			hlist_for_each_entry(pol,
					     net->xfrm.policy_bydst[dir].table + i,
					     bydst) {
				if (pol->type != type)
					continue;
				__xfrm_policy_unlink(pol, dir);
				spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
				cnt++;

				xfrm_audit_policy_delete(pol, 1, task_valid);
				xfrm_policy_kill(pol);

				spin_lock_bh(&net->xfrm.xfrm_policy_lock);
				goto again2;
			}
		}

	}
	if (!cnt)
		err = -ESRCH;
out:
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);
	return err;
}