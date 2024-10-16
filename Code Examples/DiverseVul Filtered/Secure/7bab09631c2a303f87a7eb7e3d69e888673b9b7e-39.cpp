static int xfrm_migrate_check(const struct xfrm_migrate *m, int num_migrate)
{
	int i, j;

	if (num_migrate < 1 || num_migrate > XFRM_MAX_DEPTH)
		return -EINVAL;

	for (i = 0; i < num_migrate; i++) {
		if (xfrm_addr_any(&m[i].new_daddr, m[i].new_family) ||
		    xfrm_addr_any(&m[i].new_saddr, m[i].new_family))
			return -EINVAL;

		/* check if there is any duplicated entry */
		for (j = i + 1; j < num_migrate; j++) {
			if (!memcmp(&m[i].old_daddr, &m[j].old_daddr,
				    sizeof(m[i].old_daddr)) &&
			    !memcmp(&m[i].old_saddr, &m[j].old_saddr,
				    sizeof(m[i].old_saddr)) &&
			    m[i].proto == m[j].proto &&
			    m[i].mode == m[j].mode &&
			    m[i].reqid == m[j].reqid &&
			    m[i].old_family == m[j].old_family)
				return -EINVAL;
		}
	}

	return 0;
}