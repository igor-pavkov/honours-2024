static int count_esp_combs(const struct xfrm_tmpl *t)
{
	int i, k, sz = 0;

	for (i = 0; ; i++) {
		const struct xfrm_algo_desc *ealg = xfrm_ealg_get_byidx(i);
		if (!ealg)
			break;

		if (!ealg->pfkey_supported)
			continue;

		if (!(ealg_tmpl_set(t, ealg)))
			continue;

		for (k = 1; ; k++) {
			const struct xfrm_algo_desc *aalg = xfrm_aalg_get_byidx(k);
			if (!aalg)
				break;

			if (!aalg->pfkey_supported)
				continue;

			if (aalg_tmpl_set(t, aalg))
				sz += sizeof(struct sadb_comb);
		}
	}
	return sz + sizeof(struct sadb_prop);
}