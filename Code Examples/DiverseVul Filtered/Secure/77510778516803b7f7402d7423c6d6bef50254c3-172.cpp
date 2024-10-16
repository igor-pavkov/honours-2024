GF_Err ainf_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_AssetInformationBox *ptr = (GF_AssetInformationBox *) s;

	ISOM_DECREASE_SIZE(s, 4)
	ptr->profile_version = gf_bs_read_u32(bs);
	return gf_isom_read_null_terminated_string(s, bs, s->size, &ptr->APID);
}