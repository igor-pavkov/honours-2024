static int cipso_v4_map_cat_rbm_hton(const struct cipso_v4_doi *doi_def,
				     const struct netlbl_lsm_secattr *secattr,
				     unsigned char *net_cat,
				     u32 net_cat_len)
{
	int host_spot = -1;
	u32 net_spot = CIPSO_V4_INV_CAT;
	u32 net_spot_max = 0;
	u32 net_clen_bits = net_cat_len * 8;
	u32 host_cat_size = 0;
	u32 *host_cat_array = NULL;

	if (doi_def->type == CIPSO_V4_MAP_TRANS) {
		host_cat_size = doi_def->map.std->cat.local_size;
		host_cat_array = doi_def->map.std->cat.local;
	}

	for (;;) {
		host_spot = netlbl_catmap_walk(secattr->attr.mls.cat,
					       host_spot + 1);
		if (host_spot < 0)
			break;

		switch (doi_def->type) {
		case CIPSO_V4_MAP_PASS:
			net_spot = host_spot;
			break;
		case CIPSO_V4_MAP_TRANS:
			if (host_spot >= host_cat_size)
				return -EPERM;
			net_spot = host_cat_array[host_spot];
			if (net_spot >= CIPSO_V4_INV_CAT)
				return -EPERM;
			break;
		}
		if (net_spot >= net_clen_bits)
			return -ENOSPC;
		netlbl_bitmap_setbit(net_cat, net_spot, 1);

		if (net_spot > net_spot_max)
			net_spot_max = net_spot;
	}

	if (++net_spot_max % 8)
		return net_spot_max / 8 + 1;
	return net_spot_max / 8;
}