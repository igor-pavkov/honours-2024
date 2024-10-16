GF_Err m4ds_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u8 *enc_ods;
	u32 enc_od_size;
	GF_MPEG4ExtensionDescriptorsBox *ptr = (GF_MPEG4ExtensionDescriptorsBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	enc_ods = NULL;
	enc_od_size = 0;
	e = gf_odf_desc_list_write(ptr->descriptors, &enc_ods, &enc_od_size);
	if (e) return e;
	if (enc_od_size) {
		gf_bs_write_data(bs, enc_ods, enc_od_size);
		gf_free(enc_ods);
	}
	return GF_OK;
}