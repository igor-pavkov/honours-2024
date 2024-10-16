static int xfrm_policy_migrate(struct xfrm_policy *pol,
			       struct xfrm_migrate *m, int num_migrate)
{
	struct xfrm_migrate *mp;
	int i, j, n = 0;

	write_lock_bh(&pol->lock);
	if (unlikely(pol->walk.dead)) {
		/* target policy has been deleted */
		write_unlock_bh(&pol->lock);
		return -ENOENT;
	}

	for (i = 0; i < pol->xfrm_nr; i++) {
		for (j = 0, mp = m; j < num_migrate; j++, mp++) {
			if (!migrate_tmpl_match(mp, &pol->xfrm_vec[i]))
				continue;
			n++;
			if (pol->xfrm_vec[i].mode != XFRM_MODE_TUNNEL &&
			    pol->xfrm_vec[i].mode != XFRM_MODE_BEET)
				continue;
			/* update endpoints */
			memcpy(&pol->xfrm_vec[i].id.daddr, &mp->new_daddr,
			       sizeof(pol->xfrm_vec[i].id.daddr));
			memcpy(&pol->xfrm_vec[i].saddr, &mp->new_saddr,
			       sizeof(pol->xfrm_vec[i].saddr));
			pol->xfrm_vec[i].encap_family = mp->new_family;
			/* flush bundles */
			atomic_inc(&pol->genid);
		}
	}

	write_unlock_bh(&pol->lock);

	if (!n)
		return -ENODATA;

	return 0;
}