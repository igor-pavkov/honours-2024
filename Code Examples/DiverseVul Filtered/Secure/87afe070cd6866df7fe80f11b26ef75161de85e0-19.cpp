u32 parse_brand(char *b, u32 opt)
{
	open_edit = GF_TRUE;
	switch (opt) {
	case 0: //-brand
		major_brand = GF_4CC(b[0], b[1], b[2], b[3]);
		if (b[4] == ':') {
			if (!strncmp(b+5, "0x", 2))
				sscanf(b+5, "0x%x", &minor_version);
			else
				minor_version = atoi(b + 5);
		}
		break;
	case 1: //-ab
		brand_add = (u32*)gf_realloc(brand_add, sizeof(u32) * (nb_alt_brand_add + 1));
		if (!brand_add) return 2;
		brand_add[nb_alt_brand_add] = GF_4CC(b[0], b[1], b[2], b[3]);
		nb_alt_brand_add++;
		break;
	case 2: //-rb
		brand_rem = (u32*)gf_realloc(brand_rem, sizeof(u32) * (nb_alt_brand_rem + 1));
		if (!brand_rem) return 2;
		brand_rem[nb_alt_brand_rem] = GF_4CC(b[0], b[1], b[2], b[3]);
		nb_alt_brand_rem++;
		break;
	}
	return 0;
}