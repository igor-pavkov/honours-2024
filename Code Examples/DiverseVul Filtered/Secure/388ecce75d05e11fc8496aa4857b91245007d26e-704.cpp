GF_Err stdp_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_DegradationPriorityBox *ptr = (GF_DegradationPriorityBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	for (i = 0; i < ptr->nb_entries; i++) {
		gf_bs_write_u16(bs, ptr->priorities[i]);
	}
	return GF_OK;
}