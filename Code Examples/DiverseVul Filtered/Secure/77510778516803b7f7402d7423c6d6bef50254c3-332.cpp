GF_Err stri_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_SubTrackInformationBox *ptr = (GF_SubTrackInformationBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, ptr->switch_group);
	gf_bs_write_u16(bs, ptr->alternate_group);
	gf_bs_write_u32(bs, ptr->sub_track_id);
	for (i = 0; i < ptr->attribute_count; i++) {
		gf_bs_write_u32(bs, ptr->attribute_list[i]);
	}
	return GF_OK;
}