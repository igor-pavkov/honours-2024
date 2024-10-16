static void xfrm_policy_timer(unsigned long data)
{
	struct xfrm_policy *xp = (struct xfrm_policy *)data;
	unsigned long now = get_seconds();
	long next = LONG_MAX;
	int warn = 0;
	int dir;

	read_lock(&xp->lock);

	if (unlikely(xp->walk.dead))
		goto out;

	dir = xfrm_policy_id2dir(xp->index);

	if (xp->lft.hard_add_expires_seconds) {
		long tmo = xp->lft.hard_add_expires_seconds +
			xp->curlft.add_time - now;
		if (tmo <= 0)
			goto expired;
		if (tmo < next)
			next = tmo;
	}
	if (xp->lft.hard_use_expires_seconds) {
		long tmo = xp->lft.hard_use_expires_seconds +
			(xp->curlft.use_time ? : xp->curlft.add_time) - now;
		if (tmo <= 0)
			goto expired;
		if (tmo < next)
			next = tmo;
	}
	if (xp->lft.soft_add_expires_seconds) {
		long tmo = xp->lft.soft_add_expires_seconds +
			xp->curlft.add_time - now;
		if (tmo <= 0) {
			warn = 1;
			tmo = XFRM_KM_TIMEOUT;
		}
		if (tmo < next)
			next = tmo;
	}
	if (xp->lft.soft_use_expires_seconds) {
		long tmo = xp->lft.soft_use_expires_seconds +
			(xp->curlft.use_time ? : xp->curlft.add_time) - now;
		if (tmo <= 0) {
			warn = 1;
			tmo = XFRM_KM_TIMEOUT;
		}
		if (tmo < next)
			next = tmo;
	}

	if (warn)
		km_policy_expired(xp, dir, 0, 0);
	if (next != LONG_MAX &&
	    !mod_timer(&xp->timer, jiffies + make_jiffies(next)))
		xfrm_pol_hold(xp);

out:
	read_unlock(&xp->lock);
	xfrm_pol_put(xp);
	return;

expired:
	read_unlock(&xp->lock);
	if (!xfrm_policy_delete(xp, dir))
		km_policy_expired(xp, dir, 1, 0);
	xfrm_pol_put(xp);
}