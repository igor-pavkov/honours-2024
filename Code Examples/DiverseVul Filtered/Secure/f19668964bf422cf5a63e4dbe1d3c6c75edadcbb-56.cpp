GF_Err flxs_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_AdobeFlashAccessParamsBox *ptr = (GF_AdobeFlashAccessParamsBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	if (ptr->metadata) {
		gf_bs_write_data(bs, ptr->metadata, (u32) strlen(ptr->metadata));
		gf_bs_write_u8(bs, 0); //string end
	}
	return GF_OK;
}