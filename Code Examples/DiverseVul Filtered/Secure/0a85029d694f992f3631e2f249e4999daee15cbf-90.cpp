GF_Err m4ds_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	char *enc_od;
	GF_MPEG4ExtensionDescriptorsBox *ptr = (GF_MPEG4ExtensionDescriptorsBox *)s;
	u32 od_size = (u32) ptr->size;
	if (!od_size) return GF_OK;
	enc_od = (char *)gf_malloc(sizeof(char) * od_size);
	gf_bs_read_data(bs, enc_od, od_size);
	e = gf_odf_desc_list_read((char *)enc_od, od_size, ptr->descriptors);
	gf_free(enc_od);
	return e;
}