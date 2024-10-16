GF_Err trep_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrackExtensionPropertiesBox *ptr = (GF_TrackExtensionPropertiesBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->trackID);
	return GF_OK;
}