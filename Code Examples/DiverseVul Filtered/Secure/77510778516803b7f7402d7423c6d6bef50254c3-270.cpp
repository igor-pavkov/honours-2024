GF_Err bloc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_BaseLocationBox *ptr = (GF_BaseLocationBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, (const char *) ptr->baseLocation, 256);
	gf_bs_write_data(bs, (const char *) ptr->basePurlLocation, 256);
	for (i=0; i < 64; i++ ) {
		gf_bs_write_u64(bs, 0);
	}
	return GF_OK;
}