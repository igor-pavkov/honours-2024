GF_Err dfla_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_FLACConfigBox *ptr = (GF_FLACConfigBox *) s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, ptr->data, ptr->dataSize);
	return GF_OK;
}