static int cipso_v4_map_cat_rbm_ntoh(const struct cipso_v4_doi *doi_def,
				     const unsigned char *net_cat,
				     u32 net_cat_len,
				     struct netlbl_lsm_secattr *secattr)
{
	int ret_val;
	int net_spot = -1;
	u32 host_spot = CIPSO_V4_INV_CAT;
	u32 net_clen_bits = net_cat_len * 8;
	u32 net_cat_size = 0;
	u32 *net_cat_array = NULL;

	if (doi_def->type == CIPSO_V4_MAP_TRANS) {
		net_cat_size = doi_def->map.std->cat.cipso_size;
		net_cat_array = doi_def->map.std->cat.cipso;
	}

	for (;;) {
		net_spot = netlbl_bitmap_walk(net_cat,
					      net_clen_bits,
					      net_spot + 1,
					      1);
		if (net_spot < 0) {
			if (net_spot == -2)
				return -EFAULT;
			return 0;
		}

		switch (doi_def->type) {
		case CIPSO_V4_MAP_PASS:
			host_spot = net_spot;
			break;
		case CIPSO_V4_MAP_TRANS:
			if (net_spot >= net_cat_size)
				return -EPERM;
			host_spot = net_cat_array[net_spot];
			if (host_spot >= CIPSO_V4_INV_CAT)
				return -EPERM;
			break;
		}
		ret_val = netlbl_catmap_setbit(&secattr->attr.mls.cat,
						       host_spot,
						       GFP_ATOMIC);
		if (ret_val != 0)
			return ret_val;
	}

	return -EINVAL;
}