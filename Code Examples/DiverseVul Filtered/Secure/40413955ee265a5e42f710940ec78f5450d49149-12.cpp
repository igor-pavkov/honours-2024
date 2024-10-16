static int cipso_v4_map_cat_rbm_valid(const struct cipso_v4_doi *doi_def,
				      const unsigned char *bitmap,
				      u32 bitmap_len)
{
	int cat = -1;
	u32 bitmap_len_bits = bitmap_len * 8;
	u32 cipso_cat_size;
	u32 *cipso_array;

	switch (doi_def->type) {
	case CIPSO_V4_MAP_PASS:
		return 0;
	case CIPSO_V4_MAP_TRANS:
		cipso_cat_size = doi_def->map.std->cat.cipso_size;
		cipso_array = doi_def->map.std->cat.cipso;
		for (;;) {
			cat = netlbl_bitmap_walk(bitmap,
						 bitmap_len_bits,
						 cat + 1,
						 1);
			if (cat < 0)
				break;
			if (cat >= cipso_cat_size ||
			    cipso_array[cat] >= CIPSO_V4_INV_CAT)
				return -EFAULT;
		}

		if (cat == -1)
			return 0;
		break;
	}

	return -EFAULT;
}