GF_Err databox_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_DataBox *ptr = (GF_DataBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, ptr->reserved, 32);
	if(ptr->data != NULL && ptr->dataSize > 0) {
		gf_bs_write_data(bs, ptr->data, ptr->dataSize);
	}
	return GF_OK;
}