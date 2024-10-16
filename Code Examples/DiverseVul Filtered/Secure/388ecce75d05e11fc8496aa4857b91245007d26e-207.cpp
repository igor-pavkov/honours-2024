GF_Err iods_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 descSize;
	u8 *desc;
	GF_ObjectDescriptorBox *ptr = (GF_ObjectDescriptorBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	//call our OD codec
	e = gf_odf_desc_write(ptr->descriptor, &desc, &descSize);
	if (e) return e;
	gf_bs_write_data(bs, desc, descSize);
	//and free our stuff maybe!!
	gf_free(desc);
	return GF_OK;
}